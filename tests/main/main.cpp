#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <iris/graphgen.hpp>
#include <iris/lex.hpp>
#include <iris/parse.hpp>

TEST_CASE("lex", "[lex]") {
  {
    std::string_view in = "\1";
    auto [token, out] = ns::lex(in);
    CHECK(std::get_if<ns::Error>(&token));
    CHECK(out.compare("\1") == 0);
  }
  {
    std::string_view in = "";
    auto [token, out] = ns::lex(in);
    CHECK(std::get_if<ns::Eof>(&token));
    CHECK(out.compare("") == 0);
  }
  {
    std::string_view in = "     ";
    auto [token, out] = ns::lex(in);
    CHECK(std::get_if<ns::Eof>(&token));
    CHECK(out.compare("") == 0);
  }
  {
    std::string_view in = "ident";
    auto [token, out] = ns::lex(in);
    auto ident = std::get_if<ns::Ident>(&token);
    CHECK(ident);
    CHECK(ident->data.compare("ident") == 0);
    CHECK(out.compare("") == 0);
  }
  {
    std::string_view in = ",";
    auto [token, out] = ns::lex(in);
    auto punct = std::get_if<ns::Punct>(&token);
    CHECK(punct);
    CHECK(punct->data.compare(",") == 0);
    CHECK(out.compare("") == 0);
  }
  {
    std::string_view in = "()";
    auto [token, out] = ns::lex(in);
    auto punct = std::get_if<ns::Punct>(&token);
    CHECK(punct);
    CHECK(punct->data.compare("(") == 0);
    CHECK(out.compare(")") == 0);
  }
  {
    std::string_view in = "Neg(input)";
    ns::Lexer it(in);
    {
      auto token = *it++;
      auto ident = std::get_if<ns::Ident>(&token);
      CHECK(ident);
      CHECK(ident->data.compare("Neg") == 0);
    }
    {
      auto token = *it++;
      auto punct = std::get_if<ns::Punct>(&token);
      CHECK(punct);
      CHECK(punct->data.compare("(") == 0);
    }
    {
      auto token = *it++;
      auto ident = std::get_if<ns::Ident>(&token);
      CHECK(ident);
      CHECK(ident->data.compare("input") == 0);
    }
    {
      auto token = *it++;
      auto punct = std::get_if<ns::Punct>(&token);
      CHECK(punct);
      CHECK(punct->data.compare(")") == 0);
    }
    {
      auto token = *it++;
      auto eof = std::get_if<ns::Eof>(&token);
      CHECK(eof);
    }
    {
      auto token = *it++;
      auto eof = std::get_if<ns::Eof>(&token);
      CHECK(eof);
    }
  }
}

TEST_CASE("parse", "[parse]") {
  {
    std::string_view in = "Const()";
    ns::Lexer it(in);
    auto result = ns::parse_expr(it);
    auto expr = std::get_if<ns::Expr>(&result);
    CHECK(expr);
      CHECK(expr->name.compare("Const") == 0);
      CHECK(expr->args.empty());
    }
  {
    std::string_view in = "Add(Const(), Const())";
    ns::Lexer it(in);
    auto result = ns::parse_expr(it);
    auto expr = std::get_if<ns::Expr>(&result);
    CHECK(expr);
      CHECK(expr->name.compare("Add") == 0);
      CHECK(expr->args.size() == 2);
      for (const auto& expr2 : expr->args) {
        CHECK(expr2.name.compare("Const") == 0);
        CHECK(expr2.args.empty());
      }
    }
  }

TEST_CASE("graphgen", "[graphgen]") {
  {
    std::string_view in = "Const()";
    ns::Lexer it(in);
    auto result = ns::parse_expr(it);
    if (auto expr = std::get_if<ns::Expr>(&result)) {
      auto result2 = ns::GraphGen().gen(std::move(*expr));
      auto graph = std::get_if<ns::Graph>(&result2);
      CHECK(graph);
      std::cout << '\n' << *graph << std::endl;
        CHECK(graph->nodes().size() == 1);

        const auto& node = graph->nodes()[0];
      CHECK(node->name().compare("Const") == 0);
        CHECK(node->inputs().size() == 0);
        CHECK(node->outputs().size() == 0);
        CHECK(node.use_count() == 1);
    }
  }
  {
    std::string_view in = "Add(Const(), Const())";
    ns::Lexer it(in);
    auto result = ns::parse_expr(it);
    if (auto expr = std::get_if<ns::Expr>(&result)) {
      auto result2 = ns::GraphGen().gen(std::move(*expr));
      auto graph = std::get_if<ns::Graph>(&result2);
      CHECK(graph);
      std::cout << '\n' << *graph << std::endl;
        CHECK(graph->nodes().size() == 3);

        {
          const auto& node = graph->nodes()[0];
          const auto& value = node->outputs()[0];
        CHECK(node->name().compare("Const") == 0);
          CHECK(node->inputs().size() == 0);
          CHECK(node->outputs().size() == 1);
          CHECK(node.use_count() == 1);
          CHECK(value.use_count() == 2);
        }
        {
          const auto& node = graph->nodes()[1];
          const auto& value = node->outputs()[0];
        CHECK(node->name().compare("Const") == 0);
          CHECK(node->inputs().size() == 0);
          CHECK(node->outputs().size() == 1);
          CHECK(node.use_count() == 1);
          CHECK(value.use_count() == 2);
        }
        {
          const auto& node = graph->nodes()[2];
        CHECK(node->name().compare("Add") == 0);
          CHECK(node->inputs().size() == 2);
          CHECK(node->outputs().size() == 0);
          CHECK(node.use_count() == 1);
      }
    }
  }
}
