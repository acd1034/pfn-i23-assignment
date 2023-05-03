/// @file insert_nop_after_opa.hpp
#pragma once
#include "fundamental.hpp"
#include "graph.hpp"

namespace ns {
  struct InsertNopAfterOpA {
    InsertNopAfterOpA() = default;
    explicit InsertNopAfterOpA(Graph graph) : graph_(std::move(graph)) {}

    Graph run() && {
      auto it = graph_.nodes().begin();
      while (it != graph_.nodes().end()) {
        it = run_on_node(std::move(it));
      }
      return std::move(graph_);
    }

    Graph::insert_point_t run_on_node(Graph::insert_point_t it) {
      using namespace std::string_view_literals;
      bool is_opa = (*it)->name() == "opA"sv and (*it)->outputs().size() == 1;

      if (not is_opa) {
        ++it;
        return it;
      }

      auto nop = std::make_shared<Node>(graph_.unique_id(), "NOP");
      [[maybe_unused]] std::weak_ptr<Node> weak_nop = nop;
      std::shared_ptr<Node> opa = *it;

      // ... -> opa -> next_node -> ...
      //        nop
      relink_source_node(opa, nop);
      // ... -> opa
      //        nop -> next_node -> ...
      connect_nodes(std::move(opa), nop);
      // ... -> opa -> nop -> next_node -> ...
      it = graph_.insert_node(std::move(it) + 1, std::move(nop));

      assert(weak_nop.use_count() == 1);
      return std::move(it) + 1;
    }

  private:
    Graph graph_{};
  };
} // namespace ns
