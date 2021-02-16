#include "compiler.hpp"

void Compiler::push(const Symbol& symbol) {
  m_stack.push(symbol);
  PLOGV << VAR_TO_STR(symbol);
}

Symbol Compiler::pop() {
  const auto top = m_stack.top();
  m_stack.pop();
  PLOGV << VAR_TO_STR(top);
  return top;
}

void Compiler::triplet(Op op) {
  if (m_stack.size() < 2) {
    PLOGE << "Cannot create a triplet from less than 2 values!";
    return;
  }

  const auto rhs = pop();
  const auto lhs = pop();

  std::visit(
    [this](auto&& arg_lhs, auto&& arg_rhs) {
      using L_T = std::decay_t<decltype(arg_lhs)>;
      using R_T = std::decay_t<decltype(arg_rhs)>;

      if constexpr (std::is_same_v<L_T, std::string>) {
        m_assembler.load_word("$t0", arg_lhs);
      } else if constexpr (std::is_same_v<L_T, Data>) {
        m_assembler.load_immediate("$t0", arg_lhs.to_string());
      }
      if constexpr (std::is_same_v<R_T, std::string>) {
        m_assembler.load_word("$t1", arg_rhs);
      } else if constexpr (std::is_same_v<R_T, Data>) {
        m_assembler.load_immediate("$t1", arg_rhs.to_string());
      }
    },
    lhs,
    rhs);

  switch (op) {
    case Op::Add: {
      m_assembler.add("$t0", "$t1");
      break;
    }
    case Op::Sub: {
      m_assembler.sub("$t0", "$t1");
      break;
    }
    case Op::Mul: {
      m_assembler.mul("$t0", "$t1");
      break;
    }
    case Op::Div: {
      m_assembler.div("$t0", "$t1");
      break;
    }
    default: {
      break;
    }
  }

  const auto identifier = get_random_identifier();
  store_variable(identifier, {});
  push(identifier);
}

void Compiler::assign(const std::string& identifier) {
  if (m_stack.size() < 1) {
    PLOGE << "Cannot assign 0 values!";
    return;
  }

  const auto rhs = pop();

  std::visit(
    [this](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;

      if constexpr (std::is_same_v<T, std::string>) {
        m_assembler.load_word("$t0", arg);
      } else if constexpr (std::is_same_v<T, Data>) {
        m_assembler.load_immediate("$t0", arg.to_string());
      }
    },
    rhs);

  store_variable(identifier, {});
}

void Compiler::print() {
  const auto symbol = pop();
  std::visit(
    [this](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;

      if constexpr (std::is_same_v<T, std::string>) {
        m_assembler.print(m_type_to_print[m_variables[arg].type], arg, true);
      } else if constexpr (std::is_same_v<T, Data>) {
        m_assembler.print(m_type_to_print[arg.type], arg.to_string(), false);
      }
    },
    symbol);
}

void Compiler::compile() {
  write_symbols_to_file();
  m_assembler.to_file();
}

void Compiler::write_symbols_to_file(const std::string& file_name) const {
  std::ofstream ofs(file_name);
  if (ofs.is_open()) {
    ofs << ".data" << std::endl;
    for (auto it = m_variables.begin(); it != m_variables.end(); ++it) {
      ofs << "\t\t" << it->first << ": ";
      switch (it->second.type) {
        case Type::f32: {
          ofs << ".float 0.0";
          break;
        }

        case Type::i32: {
          ofs << ".word 0";
          break;
        }

        default: {
          ofs << ".word 0";
          break;
        }
      }
      ofs << std::endl;
    }
    ofs << std::endl;
  } else {
    PLOGE << "Could not open " << file_name << " for write!" << std::endl;
  }
}

void Compiler::store_variable(const std::string& identifier,
                              const Variable& data) {
  m_assembler.store_word("$t0", identifier);
  if (m_variables.find(identifier) == m_variables.end()) {
    m_variables[identifier] = data;
  } else {
    PLOGW << "Variable already exist!";
  }
  PLOGV << "Variable stored: " << identifier;
}
