#ifndef STACK_HPP
#define STACK_HPP

#include <stack>
#include <string>
#include <variant>

class Stack {
 public:
 private:
  std::stack<std::variant<int, float, std::string>> m_stack;
};

#endif
