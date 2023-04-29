/// @file lex.hpp
#pragma once
#include <algorithm>
#include <cctype>
#include "fundamental.hpp" // Error

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
      return {Token(Ident{std::move(data)}), input.substr(pos)};
    }

    if (ispunct(input.front())) {
      return {Token(Punct{input.substr(0, 1)}), input.substr(1)};
    }

    using namespace std::string_view_literals;
    return {Token(Error{"unexpected character"sv}), input};
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
