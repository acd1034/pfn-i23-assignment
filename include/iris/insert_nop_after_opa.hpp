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
      std::shared_ptr<Node> node = *it;
      using namespace std::string_view_literals;
      bool is_opa = node->name() == "opA"sv and node->outputs().size() == 1;

      if (not is_opa) {
        return std::move(it) + 1;
      }

      // ... -> opa -> next_node -> ...
      std::shared_ptr<Node> opa = std::move(node);
      auto nop = std::make_shared<Node>(graph_.unique_id(), "NOP");
      it = graph_.insert_node(std::move(it) + 1, nop);
      // ... -> opa -(value)-> next_node -> ...
      //        nop
      std::shared_ptr<Value> value = opa->outputs()[0];
      relink_source_node(std::move(value), nop);
      // ... -> opa
      //        nop -> next_node -> ...
      connect_nodes(std::move(opa), nop);
      // ... -> opa -> nop -> next_node -> ...

      assert(nop.use_count() == 2);
      return std::move(it) + 1;
    }

  private:
    Graph graph_{};
  };
} // namespace ns
