#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <plog/Log.h>

#include <fstream>
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

enum class IO {
  PrintI = 1,
  PrintF = 2,
  PrintS = 4,
  PrintCh = 12,
  ReadI = 5,
  ReadF = 6,
  ReadS = 8
};

class Assembler {
 public:
  void load_immediate(const std::string& registry, const std::string& value) {
    binary_instruction("li", registry, value);
  }
  void load_word(const std::string& registry, const std::string& value) {
    binary_instruction("lw", registry, value);
  }
  void store_word(const std::string& registry, const std::string& value) {
    binary_instruction("sw", registry, value);
  }

  void add(const std::string& lhs_registry, const std::string& rhs_registry) {
    ternary_instruction("add", lhs_registry, lhs_registry, rhs_registry);
  }
  void sub(const std::string& lhs_registry, const std::string& rhs_registry) {
    ternary_instruction("sub", lhs_registry, lhs_registry, rhs_registry);
  }
  void mul(const std::string& lhs_registry, const std::string& rhs_registry) {
    ternary_instruction("mul", lhs_registry, lhs_registry, rhs_registry);
  }
  void div(const std::string& lhs_registry, const std::string& rhs_registry) {
    binary_instruction("div", lhs_registry, rhs_registry);
  }

  void to_file(const std::string& file_name = "a.asm") const {
    std::ofstream ofs(file_name, std::ios_base::app);
    if (ofs.is_open()) {
      ofs << ".text" << std::endl;
      for (auto&& asm_instruction : m_asm) {
        ofs << "\t\t" << asm_instruction << std::endl;
      }
    } else {
      PLOGE << "Could not open " << file_name << " for write!" << std::endl;
    }
  }

  void print(IO id, const std::string& symbol, bool is_word) {
    load_immediate("$v0", std::to_string(static_cast<int>(id)));

    auto load_fn = is_word ? &Assembler::load_word : &Assembler::load_immediate;
    switch (id) {
      case IO::PrintI:
      case IO::PrintS:
      case IO::PrintCh: {
        (this->*(load_fn))(std::string{ "$a0" }, symbol);
        break;
      }
      case IO::PrintF: {
        (this->*(load_fn))(std::string{ "$f12" }, symbol);
        break;
      }
      default: {
        break;
      }
    }
    syscall();
  }

 private:
  void unary_instruction(const std::string& instruction,
                         const std::string& value) {
    m_asm.emplace_back(instruction + " " + value);
  }
  void binary_instruction(const std::string& instruction,
                          const std::string& lhs,
                          const std::string& rhs) {
    m_asm.emplace_back(instruction + " " + lhs + ", " + rhs);
  }
  void ternary_instruction(const std::string& instruction,
                           const std::string& lhs,
                           const std::string& ms,
                           const std::string& rhs) {
    m_asm.emplace_back(instruction + " " + lhs + ", " + ms + ", " + rhs);
  }

  void syscall() {
    m_asm.emplace_back("syscall");
  }

 private:
  std::vector<std::string> m_asm;
};

#endif
