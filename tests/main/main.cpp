#include <catch2/catch_test_macros.hpp>
#include <iris/fundamental.hpp>

TEST_CASE("lex", "[lex]") {
  {
    std::string_view in = "$";
    auto [token, out] = ns::lex(in);
    CHECK(std::get_if<ns::Error>(&token));
    CHECK(out.compare("$") == 0);
  }
  {
    std::string_view in = "";
    auto [token, out] = ns::lex(in);
    CHECK(std::get_if<ns::Eof>(&token));
    CHECK(out.compare("") == 0);
  }
}
