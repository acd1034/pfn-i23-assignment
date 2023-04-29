/// @file fundamental.hpp
#pragma once
// #include <compare>
// #include <cstddef> // std::size_t, std::ptrdiff_t, std::nullptr_t
// #include <cstdint> // std::int32_t
// #include <initializer_list>
#include <algorithm>
#include <cassert>
#include <cctype>
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

  inline constexpr auto isspace = [](char c) {
    return std::isspace(static_cast<unsigned char>(c));
  };
  inline constexpr auto isalpha = [](char c) {
    return std::isalpha(static_cast<unsigned char>(c));
  };
  inline constexpr auto ispunct = [](char c) {
    return std::ispunct(static_cast<unsigned char>(c));
  };

  std::pair<Token, std::string_view> lex(std::string_view input) {
    using namespace std::string_view_literals;

    if (input.empty()) {
      return {Token(Eof{}), input};
    }

    if (isspace(input.front())) {
      auto it = std::find_if_not(input.begin(), input.end(), isspace);
      auto pos = static_cast<std::size_t>(it - input.begin());
      return lex(input.substr(pos));
    }

    if (isalpha(input.front())) {
      auto it = std::find_if_not(input.begin(), input.end(), isalpha);
      auto pos = static_cast<std::size_t>(it - input.begin());
      std::string data(input.substr(0, pos));
      return {Token(Ident{data}), input.substr(pos)};
    }

    if (ispunct(input.front())) {
      return {Token(Punct{input.substr(0, 1)}), input.substr(1)};
    }

    return {Token(Error{"Unexpected character"sv}), input};
  }
} // namespace ns
