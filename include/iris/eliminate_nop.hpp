/// @file eliminate_nop.hpp
#pragma once
#include "fundamental.hpp"
#include "graph.hpp"

namespace ns {
  struct EliminateNop {
    EliminateNop() = default;
    explicit EliminateNop(Graph graph) : graph_(std::move(graph)) {}

    Graph run() && {
      auto it = graph_.nodes().begin();
      while (it != graph_.nodes().end()) {
        it = run_on_node(std::move(it));
      }
      return std::move(graph_);
    }

    Graph::node_iterator run_on_node(Graph::node_iterator it) {
      std::shared_ptr<Node> node = *it;
      using namespace std::string_view_literals;
      bool is_nop = node->name() == "NOP"sv and node->inputs().size() == 1 and
          node->outputs().size() == 1;

      if (not is_nop) {
        return std::move(it) + 1;
      }

      // ... -> prev_node -(value)-> nop -> next_node -> ...
      std::shared_ptr<Node> nop = std::move(node);
      [[maybe_unused]] long use_count = nop.use_count();
      std::shared_ptr<Value> value = nop->inputs()[0];
      std::shared_ptr<Node> prev_node = value->source();
      disconnect_nodes(std::move(value));
      // ... -> prev_node
      //              nop -(value2)-> next_node -> ...
      std::shared_ptr<Value> value2 = nop->outputs()[0];
      relink_source_node(std::move(value2), std::move(prev_node));
      // ... -> prev_node -> next_node -> ...
      //              nop
      it = graph_.erase_node(std::move(it));
      // ... -> prev_node -> next_node -> ...

      assert(nop.use_count() == use_count - 1);
      return it;
    }

  private:
    Graph graph_{};
  };
} // namespace ns
