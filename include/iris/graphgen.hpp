/// @file graphgen.hpp
#pragma once
#include "fundamental.hpp" // Error
#include "graph.hpp"
#include "parse.hpp" // Expr

namespace ns {
  struct GraphGen {
    GraphGen() = default;
    explicit GraphGen(Graph graph) : builder_(std::move(graph)) {}

    std::variant<Graph, Error> gen(Expr expr) && {
      NS_RESULT_TRY(_n, gen_expr(std::move(expr)));
      return std::move(builder_).graph();
    }

    std::variant<std::shared_ptr<Node>, Error> gen_expr(Expr expr) {
      return gen_call(std::move(expr));
    }

    std::variant<std::shared_ptr<Node>, Error> gen_call(Expr expr) {
      std::vector<std::shared_ptr<Node>> nodes{};
      for (auto&& arg : expr.args) {
        NS_RESULT_TRY(node, gen_expr(std::move(arg)));
        nodes.push_back(std::move(node));
      }
      return builder_.build_node(std::move(expr.name), std::move(nodes));
    }

  private:
    GraphBuilder builder_{};
  };
} // namespace ns
