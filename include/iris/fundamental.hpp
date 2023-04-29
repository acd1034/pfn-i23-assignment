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
#include <iterator>
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
    friend bool operator==(const Ident&, const Ident&) = default;
  };
  struct Punct {
    std::string_view data;
    friend bool operator==(const Punct&, const Punct&) = default;
  };
  struct Eof {
    friend bool operator==(const Eof&, const Eof&) = default;
  };
  struct Error {
    std::string_view msg;
    friend bool operator==(const Error&, const Error&) = default;
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

  struct Lexer {
  private:
    std::string_view input_{};
    Token token_{};

  public:
    using difference_type = std::ptrdiff_t;
    using value_type = Token;
    using iterator_concept = std::forward_iterator_tag;

    Lexer() = default;

    explicit Lexer(std::string_view input) {
      auto [token, output] = lex(input);
      input_ = output;
      token_ = token;
    }

    value_type operator*() const { return token_; }

    Lexer& operator++() {
      auto [token, output] = lex(input_);
      input_ = output;
      token_ = token;
      return *this;
    }

    Lexer operator++(int) {
      Lexer tmp = *this;
      ++*this;
      return tmp;
    }

    friend bool operator==(const Lexer&, const Lexer&) = default;
  };

  static_assert(std::forward_iterator<Lexer>);
} // namespace ns
