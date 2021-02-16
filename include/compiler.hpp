#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <plog/Log.h>

#include <cstddef>
#include <fstream>
#include <stack>
#include <unordered_map>

#include "assembler.hpp"
#include "helpers.hpp"

class Compiler {
 public:
  void push(const Symbol& symbol);
  Symbol pop();

  void triplet(Op op);
  void assign(const std::string& identifier);
  void print();

  void compile();
  void write_symbols_to_file(const std::string& file_name = "a.asm") const;

 private:
  void store_variable(const std::string& identifier, const Variable& data);

 private:
  Assembler m_assembler;

  std::stack<Symbol> m_stack;
  std::unordered_map<std::string, Variable> m_variables;

  std::unordered_map<Type, IO> m_type_to_print = { { Type::i32, IO::PrintI },
                                                   { Type::f32, IO::PrintF } };
  std::unordered_map<Type, IO> m_type_to_read = { { Type::i32, IO::ReadI },
                                                  { Type::f32, IO::ReadF } };
};

#endif
