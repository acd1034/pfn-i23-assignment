#include <catch2/catch_test_macros.hpp>
#include <iris/fundamental.hpp>

TEST_CASE("lexer", "[lexer]") {
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
  }
}
