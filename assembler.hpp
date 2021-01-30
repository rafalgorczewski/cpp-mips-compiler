#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <fstream>
#include <string>
#include <type_traits>
#include <vector>

using namespace std::literals::string_literals;

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

  void move(const std::string& lhs_registry, const std::string& rhs_registry) {
    binary_instruction("move", lhs_registry, rhs_registry);
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

 private:
  std::vector<std::string> m_asm;
};

#endif
