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

    Graph::insert_point_t run_on_node(Graph::insert_point_t it) {
      using namespace std::string_view_literals;
      bool is_nop = (*it)->name() == "NOP"sv and (*it)->inputs().size() == 1
                    and (*it)->outputs().size() == 1;

      if (not is_nop) {
        ++it;
        return it;
      }

      std::shared_ptr<Node> nop = *it;
      [[maybe_unused]] std::weak_ptr<Node> weak_nop = nop;
      std::shared_ptr<Node> prev_node = nop->inputs()[0]->source();

      // ... -> prev_node -> nop -> next_node -> ...
      disconnect_nodes(prev_node, nop);
      // ... -> prev_node
      //              nop -> next_node -> ...
      relink_source_node(std::move(nop), std::move(prev_node));
      // ... -> prev_node -> next_node -> ...
      //              nop
      it = graph_.erase_node(std::move(it));

      assert(weak_nop.expired());
      return it;
    }

  private:
    Graph graph_{};
  };
} // namespace ns
