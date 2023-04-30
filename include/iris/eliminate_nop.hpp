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
      disconnect_and_relink(*it);
      // graph_ から nop を削除
      it = graph_.erase_node(std::move(it));
      assert(nop.expired());
      return it;
    }

  private:
    void disconnect_and_relink(std::shared_ptr<Node> nop) const {
      // ... -> prev_node -(value)-> nop -(value2)-> next_node -> ...
      std::shared_ptr<Node> prev_node = nop->inputs()[0]->source();

      // disconnect(prev_node, nop)
      assert(prev_node->outputs().size() == 1 and nop->outputs().size() == 1);
      [[maybe_unused]] std::weak_ptr<Value> value = nop->inputs()[0];
      // prev_node.outputs[0] 削除
      prev_node->clear_output();
      // nop.input[0] 削除
      nop->clear_input();
      assert(value.expired());

      // relink(nop, prev_node)
      // nop.outputs[0].source を prev_node に変更する
      assert(nop->outputs().size() == 1);
      std::shared_ptr<Value> value2 = nop->outputs()[0];
      // nop.outputs[0] 削除
      nop->clear_output();
      // prev_node.outputs 追加
      prev_node->append_output(value2);
      // value2.source 更新
      value2->set_source(prev_node);

      // この時点のグラフ:
      // ... -> prev_node -(value2)-> next_node -> ...
    }

    Graph graph_{};
  };
} // namespace ns
