/// @file eliminate_nop.hpp
#pragma once
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

      [[maybe_unused]] std::weak_ptr<Node> nop = *it;
      disconnect_and_relink_values(*it);
      // graph_ から nop を削除
      it = graph_.erase_node(it);
      assert(nop.expired());
      return it;
    }

  private:
    void disconnect_and_relink_values(std::shared_ptr<Node> nop) const {
      // ... -> prev_node -(value)-> nop -(value2)-> next_node -> ...
      [[maybe_unused]] std::weak_ptr<Value> value = nop->inputs()[0];
      std::shared_ptr<Node> prev_node = nop->inputs()[0]->source();

      // prev_node.output[0] 削除
      prev_node->clear_output();
      // nop.input[0] 削除
      nop->clear_input();
      assert(value.expired());

      // nop.output[0] 削除
      nop->clear_output();

      std::shared_ptr<Value> value2 = nop->outputs()[0];
      // value2.source 更新
      value2->set_source(prev_node);
      // prev_node.output 追加
      prev_node->append_output(value2);

      // この時点のグラフ:
      // ... -> prev_node -(value2)-> next_node -> ...
    }

    Graph graph_{};
  };
} // namespace ns
