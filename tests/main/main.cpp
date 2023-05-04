#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <iris/eliminate_nop.hpp>
#include <iris/graphgen.hpp>
#include <iris/insert_nop_after_opa.hpp>
#include <iris/lex.hpp>
#include <iris/memory_leak_analyzer.hpp>
#include <iris/memory_usage_analyzer.hpp>
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
    std::string_view in = "ident_0";
    auto [token, out] = ns::lex(in);
    auto ident = std::get_if<ns::Ident>(&token);
    CHECK(ident);
    CHECK(ident->data.compare("ident_0") == 0);
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
  SECTION("Const()") {
    std::string_view in = "Const()";
    ns::Lexer it(in);
    auto result = ns::parse_expr(it);
    auto expr = std::get_if<ns::Expr>(&result);
    REQUIRE(expr);

    auto result2 = ns::GraphGen().gen(std::move(*expr));
    auto graph = std::get_if<ns::Graph>(&result2);
    REQUIRE(graph);
    std::cout << '\n' << *graph << std::endl;

    auto stat = ns::MemoryLeakAnalyzer(*graph).run();
    CHECK(stat.pos == graph->nodes().end());

    CHECK(graph->nodes().size() == 1);

    {
      const auto& node = graph->nodes()[0];
      CHECK(node->name().compare("Const") == 0);
      CHECK(node->inputs().size() == 0);
      CHECK(node->outputs().size() == 0);
      CHECK(node.use_count() == 1);
    }
  }
  SECTION("Add(Const(), Const())") {
    std::string_view in = "Add(Const(), Const())";
    ns::Lexer it(in);
    auto result = ns::parse_expr(it);
    auto expr = std::get_if<ns::Expr>(&result);
    REQUIRE(expr);

    auto result2 = ns::GraphGen().gen(std::move(*expr));
    auto graph = std::get_if<ns::Graph>(&result2);
    REQUIRE(graph);
    std::cout << '\n' << *graph << std::endl;

    auto stat = ns::MemoryLeakAnalyzer(*graph).run();
    CHECK(stat.pos == graph->nodes().end());

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

TEST_CASE("eliminate_nop", "[eliminate_nop]") {
  SECTION("Add(NOP(Const()), Const())") {
    std::string_view in = "Add(NOP(Const()), Const())";
    ns::Lexer it(in);
    auto result = ns::parse_expr(it);
    auto expr = std::get_if<ns::Expr>(&result);
    REQUIRE(expr);

    auto result2 = ns::GraphGen().gen(std::move(*expr));
    auto graph = std::get_if<ns::Graph>(&result2);
    REQUIRE(graph);
    std::cout << '\n' << *graph << std::endl;

    std::weak_ptr<ns::Node> nop = graph->nodes()[1];
    std::weak_ptr<ns::Value> src = nop.lock()->inputs()[0];
    std::weak_ptr<ns::Value> tgt = nop.lock()->outputs()[0];
    CHECK(nop.lock()->name().compare("NOP") == 0);

    auto graph_opt = ns::EliminateNop(std::move(*graph)).run();
    std::cout << '\n' << graph_opt << std::endl;

    auto stat = ns::MemoryLeakAnalyzer(*graph).run();
    CHECK(stat.pos == graph->nodes().end());
    CHECK(nop.expired());
    CHECK(src.expired());
    CHECK(tgt.use_count() == 2);

    CHECK(graph_opt.nodes().size() == 3);
    CHECK(graph_opt.nodes()[0]->name().compare("Const") == 0);
    CHECK(graph_opt.nodes()[1]->name().compare("Const") == 0);
    CHECK(graph_opt.nodes()[2]->name().compare("Add") == 0);
  }
  SECTION("Modified Add(NOP(Const()))") {
    std::string_view in = "Add(NOP(Const()))";
    ns::Lexer it(in);
    auto result = ns::parse_expr(it);
    auto expr = std::get_if<ns::Expr>(&result);
    REQUIRE(expr);

    auto result2 = ns::GraphGen().gen(std::move(*expr));
    auto graph = std::get_if<ns::Graph>(&result2);
    REQUIRE(graph);
    ns::connect_nodes(graph->nodes()[0], graph->nodes()[2]);
    std::cout << '\n' << *graph << std::endl;

    std::weak_ptr<ns::Node> nop = graph->nodes()[1];
    std::weak_ptr<ns::Value> src = nop.lock()->inputs()[0];
    std::weak_ptr<ns::Value> tgt = nop.lock()->outputs()[0];
    CHECK(nop.lock()->name().compare("NOP") == 0);

    auto graph_opt = ns::EliminateNop(std::move(*graph)).run();
    std::cout << '\n' << graph_opt << std::endl;

    auto stat = ns::MemoryLeakAnalyzer(*graph).run();
    CHECK(stat.pos == graph->nodes().end());
    CHECK(nop.expired());
    CHECK(src.expired());
    CHECK(tgt.use_count() == 2);

    CHECK(graph_opt.nodes().size() == 2);
    CHECK(graph_opt.nodes()[0]->name().compare("Const") == 0);
    CHECK(graph_opt.nodes()[1]->name().compare("Add") == 0);
  }
}

TEST_CASE("insert_nop_after_opa", "[insert_nop_after_opa]") {
  SECTION("Add(opA(), Const())") {
    std::string_view in = "Add(opA(), Const())";
    ns::Lexer it(in);
    auto result = ns::parse_expr(it);
    auto expr = std::get_if<ns::Expr>(&result);
    REQUIRE(expr);

    auto result2 = ns::GraphGen().gen(std::move(*expr));
    auto graph = std::get_if<ns::Graph>(&result2);
    REQUIRE(graph);
    std::cout << '\n' << *graph << std::endl;

    std::weak_ptr<ns::Node> opa = graph->nodes()[0];
    std::weak_ptr<ns::Value> tgt = opa.lock()->outputs()[0];
    CHECK(opa.lock()->name().compare("opA") == 0);

    auto graph_opt = ns::InsertNopAfterOpA(std::move(*graph)).run();
    std::cout << '\n' << graph_opt << std::endl;

    auto stat = ns::MemoryLeakAnalyzer(*graph).run();
    CHECK(stat.pos == graph->nodes().end());
    CHECK(opa.use_count() == 1);
    CHECK(tgt.use_count() == 2);

    CHECK(graph_opt.nodes().size() == 4);
    CHECK(graph_opt.nodes()[0]->name().compare("opA") == 0);
    CHECK(graph_opt.nodes()[1]->name().compare("NOP") == 0);
    CHECK(graph_opt.nodes()[2]->name().compare("Const") == 0);
    CHECK(graph_opt.nodes()[3]->name().compare("Add") == 0);
  }
  SECTION("Modified Add(opA(Const()))") {
    std::string_view in = "Add(opA(Const()))";
    ns::Lexer it(in);
    auto result = ns::parse_expr(it);
    auto expr = std::get_if<ns::Expr>(&result);
    REQUIRE(expr);

    auto result2 = ns::GraphGen().gen(std::move(*expr));
    auto graph = std::get_if<ns::Graph>(&result2);
    REQUIRE(graph);
    ns::connect_nodes(graph->nodes()[0], graph->nodes()[2]);
    std::cout << '\n' << *graph << std::endl;

    std::weak_ptr<ns::Node> opa = graph->nodes()[1];
    std::weak_ptr<ns::Value> tgt = opa.lock()->outputs()[0];
    CHECK(opa.lock()->name().compare("opA") == 0);

    auto graph_opt = ns::InsertNopAfterOpA(std::move(*graph)).run();
    std::cout << '\n' << graph_opt << std::endl;

    auto stat = ns::MemoryLeakAnalyzer(*graph).run();
    CHECK(stat.pos == graph->nodes().end());
    CHECK(opa.use_count() == 1);
    CHECK(tgt.use_count() == 2);

    CHECK(graph_opt.nodes().size() == 4);
    CHECK(graph_opt.nodes()[0]->name().compare("Const") == 0);
    CHECK(graph_opt.nodes()[1]->name().compare("opA") == 0);
    CHECK(graph_opt.nodes()[2]->name().compare("NOP") == 0);
    CHECK(graph_opt.nodes()[3]->name().compare("Add") == 0);
  }
}

TEST_CASE("memory_usage_analyzer", "[memory_usage_analyzer]") {
  SECTION("Op(Op(Op(Op(Op()))))") {
    std::string_view in = "Op(Op(Op(Op(Op()))))";
    ns::Lexer it(in);
    auto result = ns::parse_expr(it);
    auto expr = std::get_if<ns::Expr>(&result);
    REQUIRE(expr);

    auto result2 = ns::GraphGen().gen(std::move(*expr));
    auto graph = std::get_if<ns::Graph>(&result2);
    REQUIRE(graph);
    std::cout << '\n' << *graph << std::endl;

    auto stat = ns::MemoryUsageAnalyzer(*graph).run();
    CHECK(stat.last_uses[0] == 1);
    CHECK(stat.last_uses[1] == 2);
    CHECK(stat.last_uses[2] == 3);
    CHECK(stat.last_uses[3] == 4);
    CHECK(stat.last_uses[4] == 4);
    CHECK(stat.usages[0] == 1);
    CHECK(stat.usages[1] == 2);
    CHECK(stat.usages[2] == 2);
    CHECK(stat.usages[3] == 2);
    CHECK(stat.usages[4] == 2);
  }
  SECTION("Modified Op(Op(Op(Op(Op()))))") {
    std::string_view in = "Op(Op(Op(Op(Op()))))";
    ns::Lexer it(in);
    auto result = ns::parse_expr(it);
    auto expr = std::get_if<ns::Expr>(&result);
    REQUIRE(expr);

    auto result2 = ns::GraphGen().gen(std::move(*expr));
    auto graph = std::get_if<ns::Graph>(&result2);
    REQUIRE(graph);
    ns::connect_nodes(graph->nodes()[0], graph->nodes()[4]);
    ns::connect_nodes(graph->nodes()[1], graph->nodes()[3]);
    std::cout << '\n' << *graph << std::endl;

    auto stat = ns::MemoryUsageAnalyzer(*graph).run();
    CHECK(stat.last_uses[0] == 4);
    CHECK(stat.last_uses[1] == 3);
    CHECK(stat.last_uses[2] == 3);
    CHECK(stat.last_uses[3] == 4);
    CHECK(stat.last_uses[4] == 4);
    CHECK(stat.usages[0] == 1);
    CHECK(stat.usages[1] == 2);
    CHECK(stat.usages[2] == 3);
    CHECK(stat.usages[3] == 4);
    CHECK(stat.usages[4] == 3);
  }
}
