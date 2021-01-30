#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <plog/Log.h>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <optional>
#include <random>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>

#include "assembler.hpp"

#define VARIANTLOG(s, x) std::visit([&](auto&& arg) { PLOGV << s << arg; }, x)

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

using ValueType = std::variant<int, float, std::string>;
enum class Operator { Add, Sub, Mul, Div };

struct Variable {
  std::string identifier;
  ValueType value;
  std::optional<std::size_t> count;
  void* address = nullptr;
};

std::string variant_to_string(const ValueType& value) {
  return std::visit(
    overloaded{ [](const std::string&) -> std::string { return ""; },
                [](auto&& arg) -> std::string { return std::to_string(arg); } },
    value);
}

class Compiler {
 public:
  void push_variable(const std::string& var) {
    m_variables.insert({ var, Variable{ var } });
    m_stack.push(Variable{ var });
    PLOGV << "Stack var push: " << var;
  }

  void push(const ValueType& value) {
    m_stack.push({ "", value });
    VARIANTLOG("Stack push: ", value);
  }

  Variable pop() {
    const auto top = m_stack.top();
    m_stack.pop();
    VARIANTLOG("Stack pop: VAR: " + top.identifier + ", VAL: ", top.value);
    return top;
  }

  template <Operator Op>
  void expression_triplet() {
    const auto rhs_var = pop();
    const auto lhs_var = pop();

    if (lhs_var.identifier.empty()) {
      m_assembler.load_immediate("$t0", variant_to_string(lhs_var.value));
    } else {
      m_assembler.load_word("$t0", lhs_var.identifier);
    }

    if (rhs_var.identifier.empty()) {
      m_assembler.load_immediate("$t1", variant_to_string(rhs_var.value));
    } else {
      m_assembler.load_word("$t1", rhs_var.identifier);
    }

    const auto rhs = rhs_var.identifier.empty()
                       ? rhs_var.value
                       : m_variables[rhs_var.identifier].value;
    const auto lhs = lhs_var.identifier.empty()
                       ? lhs_var.value
                       : m_variables[lhs_var.identifier].value;

    ValueType result;

    if constexpr (Op == Operator::Add) {
      PLOGV << "Triplet ADD";
      result = std::visit(
        overloaded{
          [this](const std::string& lhs, const std::string& rhs) -> ValueType {
            m_assembler.add("$t0", "$t1");
            return lhs + rhs;
          },
          [](auto&&, const std::string&) -> ValueType { return {}; },
          [](const std::string&, auto&&) -> ValueType { return {}; },
          [this](auto&& lhs, auto&& rhs) -> ValueType {
            m_assembler.add("$t0", "$t1");
            return lhs + rhs;
          } },
        lhs,
        rhs);
    } else if constexpr (Op == Operator::Sub) {
      PLOGV << "Triplet SUB";
      result = std::visit(
        overloaded{ [](const std::string& lhs,
                       const std::string& rhs) -> ValueType { return {}; },
                    [](auto&&, const std::string&) -> ValueType { return {}; },
                    [](const std::string&, auto&&) -> ValueType { return {}; },
                    [this](auto&& lhs, auto&& rhs) -> ValueType {
                      m_assembler.sub("$t0", "$t1");
                      return lhs - rhs;
                    } },
        lhs,
        rhs);
    } else if constexpr (Op == Operator::Mul) {
      PLOGV << "Triplet MUL";
      result = std::visit(
        overloaded{ [](const std::string& lhs,
                       const std::string& rhs) -> ValueType { return {}; },
                    [](auto&&, const std::string&) -> ValueType { return {}; },
                    [](const std::string&, auto&&) -> ValueType { return {}; },
                    [this](auto&& lhs, auto&& rhs) -> ValueType {
                      m_assembler.mul("$t0", "$t1");
                      return lhs * rhs;
                    } },
        lhs,
        rhs);
    } else if constexpr (Op == Operator::Div) {
      PLOGV << "Triplet DIV";
      result = std::visit(
        overloaded{ [](const std::string& lhs,
                       const std::string& rhs) -> ValueType { return {}; },
                    [](auto&&, const std::string&) -> ValueType { return {}; },
                    [](const std::string&, auto&&) -> ValueType { return {}; },
                    [this](auto&& lhs, auto&& rhs) -> ValueType {
                      m_assembler.div("$t0", "$t1");
                      return lhs / rhs;
                    } },
        lhs,
        rhs);
    }

    if (result.valueless_by_exception()) {
      PLOGE << "Wrong result!";
    } else {
      const auto identifier = random_identifier();
      m_assembler.store_word("$t0", identifier);
      const auto variable = Variable{ identifier, result };
      m_stack.push(variable);
      m_variables.insert({ identifier, variable });
    }
  }

  void assign() {
    pop();
    const auto var = pop();
    m_assembler.store_word("$t0", var.identifier);

    PLOGV << "Assigned to: " << var.identifier;
  }

  void compile() const {
    write_symbols_to_file();
    m_assembler.to_file();
  }

 private:
  std::string random_identifier() const {
    std::string characters =
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(characters.begin(), characters.end(), generator);

    return "temp_" + characters.substr(0, 16);
  }

  void write_symbols_to_file(const std::string& file_name = "a.asm") const {
    std::ofstream ofs(file_name);
    if (ofs.is_open()) {
      ofs << ".data" << std::endl;
      for (auto&& [key, value] : m_variables) {
        ofs << "\t\t" << key << ": "
            << std::visit(
                 overloaded{
                   [](const std::string&) -> std::string { return ""; },
                   [](int) -> std::string { return ".word 0"; },
                   [](float) -> std::string { return ".word 0"; } },
                 value.value)
            << std::endl;
      }
      ofs << std::endl;
    } else {
      PLOGE << "Could not open " << file_name << " for write!" << std::endl;
    }
  }

 private:
  std::stack<Variable> m_stack;
  std::unordered_map<std::string, Variable> m_variables;
  Assembler m_assembler;
};

#endif
