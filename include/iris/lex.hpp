/// @file lex.hpp
#pragma once
#include <algorithm> // std::find_if_not
#include <cctype>
#include "fundamental.hpp"

namespace ns {
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
  struct Invalid {
    std::string_view data;
    friend bool operator==(const Invalid&, const Invalid&) = default;
  };
  using Token = std::variant<Ident, Punct, Eof, Invalid>;

  inline constexpr auto isspace = [](char c) -> bool {
    return std::isspace(static_cast<unsigned char>(c));
  };
  inline constexpr auto isident0 = [](char c) -> bool {
    return std::isalpha(static_cast<unsigned char>(c)) or c == '_';
  };
  inline constexpr auto isident1 = [](char c) -> bool {
    return std::isalnum(static_cast<unsigned char>(c)) or c == '_';
  };
  inline constexpr auto ispunct = [](char c) -> bool {
    return std::ispunct(static_cast<unsigned char>(c));
  };

  std::pair<Token, std::string_view> lex(std::string_view input) {
    if (input.empty()) {
      return {Token(Eof{}), input};
    }

    if (isspace(input[0])) {
      auto it = std::find_if_not(input.begin(), input.end(), isspace);
      auto pos = static_cast<std::size_t>(it - input.begin());
      return lex(input.substr(pos));
    }

    if (isident0(input[0])) {
      auto it = std::find_if_not(input.begin(), input.end(), isident1);
      auto pos = static_cast<std::size_t>(it - input.begin());
      std::string data(input.substr(0, pos));
      return {Token(Ident{std::move(data)}), input.substr(pos)};
    }

    if (ispunct(input[0])) {
      return {Token(Punct{input.substr(0, 1)}), input.substr(1)};
    }

    return {Token(Invalid{input.substr(0, 1)}), input.substr(1)};
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
      input_ = std::move(output);
      token_ = std::move(token);
    }

    value_type operator*() const { return token_; }

    Lexer& operator++() {
      auto [token, output] = lex(input_);
      input_ = std::move(output);
      token_ = std::move(token);
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
