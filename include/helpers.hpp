#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <plog/Log.h>

#include <cstddef>
#include <functional>
#include <optional>
#include <random>
#include <string>
#include <variant>
#include <vector>

#define VAR_TO_STR(symbol)                            \
  std::visit(                                         \
    [this](auto&& arg) {                              \
      using T = std::decay_t<decltype(arg)>;          \
      if constexpr (std::is_same_v<T, std::string>) { \
        return arg;                                   \
      } else if constexpr (std::is_same_v<T, Data>) { \
        return arg.to_string();                       \
      }                                               \
    },                                                \
    symbol);

enum class Op { Add, Sub, Mul, Div, Ass };

enum class Type { i32, f32 };
using Bytes = std::vector<bool>;

struct Data {
  Data(float v) : bytes(32), type(Type::f32) {
    std::size_t* number = reinterpret_cast<std::size_t*>(&v);
    for (int i = 0; i < bytes.size(); ++i) {
      bytes[i] = ((*number) >> i) & 1;
    }
  }
  Data(int v) : bytes(32), type(Type::i32) {
    for (int i = 0; i < bytes.size(); ++i) {
      bytes[i] = (v >> i) & 1;
    }
  }

  template <typename T>
  operator T() const {
    std::size_t number = 0;
    for (int i = 0; i < bytes.size(); ++i) {
      number |= bytes[i] << i;
    }
    return *reinterpret_cast<T*>(&number);
  }
  std::string to_string() const {
    switch (type) {
      default:
        PLOGE << "Unknown type!";
      case Type::i32: {
        return std::to_string(static_cast<int>(*this));
        break;
      }
      case Type::f32: {
        return std::to_string(static_cast<float>(*this));
        break;
      }
    }
  }

  Bytes bytes;
  Type type;
};

using Symbol = std::variant<std::string, Data>;

struct Variable {
  Type type;
  void* address;
  std::size_t count;
};

inline std::string get_random_identifier() {
  std::string characters =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

  std::random_device rd;
  std::mt19937 generator(rd());

  std::shuffle(characters.begin(), characters.end(), generator);

  return "temp_" + characters.substr(0, 16);
}

#endif
