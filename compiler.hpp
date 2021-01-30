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

#define VARIANTLOG(s, x) std::visit([](auto&& arg) { PLOGV << s << arg; }, x)

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
  void* address;
  std::optional<std::size_t> count;
};

class Compiler {
 public:
  void push(const ValueType& value) {
    m_stack.push(value);
    VARIANTLOG("Stack push: ", value);
  }

  ValueType pop() {
    const auto top = m_stack.top();
    m_stack.pop();
    VARIANTLOG("Stack pop: ", top);
    return top;
  }

  template <Operator Op>
  void expression_triplet() {
    const auto rhs = pop();
    const auto lhs = pop();

    ValueType result;

    if constexpr (Op == Operator::Add) {
      PLOGV << "Triplet ADD";
      result = std::visit(
        overloaded{
          [this](const std::string& lhs, const std::string& rhs) -> ValueType {
            m_assembler.load_immediate("$t0", lhs);
            m_assembler.load_immediate("$t1", rhs);
            m_assembler.add("$t0", "$t1");
            return lhs + rhs;
          },
          [](auto&&, const std::string&) -> ValueType { return {}; },
          [](const std::string&, auto&&) -> ValueType { return {}; },
          [this](auto&& lhs, auto&& rhs) -> ValueType {
            m_assembler.load_immediate("$t0", lhs);
            m_assembler.load_immediate("$t1", rhs);
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
                      m_assembler.load_immediate("$t0", lhs);
                      m_assembler.load_immediate("$t1", rhs);
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
                      m_assembler.load_immediate("$t0", lhs);
                      m_assembler.load_immediate("$t1", rhs);
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
                      m_assembler.load_immediate("$t0", lhs);
                      m_assembler.load_immediate("$t1", rhs);
                      m_assembler.div("$t0", "$t1");
                      return lhs / rhs;
                    } },
        lhs,
        rhs);
    }

    std::visit([](auto&& arg) {}, result);

    if (result.valueless_by_exception()) {
      PLOGE << "Wrong result!";
    } else {
      m_stack.push(std::move(result));
    }
  }

  void compile() {
    m_assembler.to_file();
  }

 private:
  std::string random_identifier() {
    std::string characters =
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(characters.begin(), characters.end(), generator);

    return "temp" + characters.substr(0, 16);
  }

 private:
  std::stack<ValueType> m_stack;
  std::unordered_map<std::string, Variable> m_variables;
  Assembler m_assembler;
};

#endif
