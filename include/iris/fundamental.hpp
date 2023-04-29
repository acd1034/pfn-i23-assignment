/// @file fundamental.hpp
#pragma once
// #include <compare>
// #include <cstddef> // std::size_t, std::ptrdiff_t, std::nullptr_t
// #include <cstdint> // std::int32_t
// #include <initializer_list>
#include <algorithm>
#include <cassert>
#include <concepts>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace ns {
  // ----- lexer -----

  struct Ident {
    std::string data;
  };
  struct Punct {
    std::string_view data;
  };
  struct Eof {};
  struct Error {
    std::string_view msg;
  };
  using Token = std::variant<Ident, Punct, Eof, Error>;

  std::pair<Token, std::string_view> lex(std::string_view input) {
    using namespace std::string_view_literals;

    if (input.empty()) {
      return {Token(Eof{}), input};
    }

    return {Token(Error{"Unexpected character"sv}), input};
  }
} // namespace ns
