/// @file parse.hpp
#pragma once
#include <optional>
#include <vector>
#include "fundamental.hpp" // Error
#include "lex.hpp"         // Token

namespace ns {
  struct Expr {
    std::string name{};
    std::vector<Expr> args{};
  };

  std::optional<Ident> consume_ident(Lexer& it) {
    Token tok = *it;
    if (auto ident = std::get_if<Ident>(&tok)) {
      ++it;
      return std::move(*ident);
    } else {
      return std::nullopt;
    }
  }

  bool consume_punct(Lexer& it, std::string_view punct) {
    if (Token tok = *it; tok == Token(Punct{punct})) {
      ++it;
      return true;
    } else {
      return false;
    }
  }

  bool consume_eof(Lexer& it) {
    Token tok = *it;
    if (std::get_if<Eof>(&tok)) {
      ++it;
      return true;
    } else {
      return false;
    }
  }

  std::variant<Ident, Error> expect_ident(Lexer& it) {
    Token tok = *it;
    if (auto ident = std::get_if<Ident>(&tok)) {
      ++it;
      return std::move(*ident);
    } else {
      using namespace std::string_view_literals;
      return Error{"unexpected token, expecting identifier"sv};
    }
  }

  std::variant<std::monostate, Error> //
  expect_punct(Lexer& it, std::string_view punct) {
    if (Token tok = *it; tok == Token(Punct{punct})) {
      ++it;
      return std::monostate{};
    } else {
      using namespace std::string_view_literals;
      return Error{"unexpected token, expecting punctuator"sv};
    }
  }

  std::variant<std::monostate, Error> expect_eof(Lexer& it) {
    Token tok = *it;
    if (std::get_if<Eof>(&tok)) {
      ++it;
      return std::monostate{};
    } else {
      using namespace std::string_view_literals;
      return Error{"unexpected token, expecting EOF"sv};
    }
  }

  /* fun_args = (expr ("," expr)*)? ")"
   * primary  = ident "(" fun_args
   * expr     = primary
   */

  std::variant<Expr, Error> parse_expr(Lexer& it);

  // fun_args = (expr ("," expr)*)? ")"
  std::variant<std::vector<Expr>, Error> parse_fun_args(Lexer& it) {
    std::vector<Expr> args{};

    if (consume_punct(it, ")")) {
      return args;
    }

    NS_RESULT_AUTO(expr, parse_expr(it));
    args.push_back(std::move(expr));
    while (not consume_punct(it, ")")) {
      NS_RESULT_AUTO(_p, expect_punct(it, ","));
      NS_RESULT_AUTO(expr2, parse_expr(it));
      args.push_back(std::move(expr2));
    }

    return args;
  }

  // primary  = ident "(" fun_args
  std::variant<Expr, Error> parse_primary(Lexer& it) {
    NS_RESULT_AUTO(name, expect_ident(it));
    NS_RESULT_AUTO(_p, expect_punct(it, "("));
    NS_RESULT_AUTO(args, parse_fun_args(it));
    return Expr{std::move(name.data), std::move(args)};
  }

  // expr     = primary
  std::variant<Expr, Error> parse_expr(Lexer& it) { return parse_primary(it); }
} // namespace ns
