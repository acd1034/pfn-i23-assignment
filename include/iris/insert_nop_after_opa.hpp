/// @file insert_nop_after_opa.hpp
#pragma once
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
      relink_and_connect(*it, nop);
      ++it;
      it = graph_.insert_node(std::move(it), std::move(nop));
      assert(it->use_count() == 1);
      ++it;
      return it;
    }

  private:
    void relink_and_connect(std::shared_ptr<Node> opa,
                            std::shared_ptr<Node> nop) const {
      // ... -> opa -(value)-> next_node -> ...

      // relink(opa, nop)
      // opa.outputs[0].source を nop に変更する
      assert(opa->outputs().size() == 1);
      std::shared_ptr<Value> value = opa->outputs()[0];
      // opa.outputs[0] 削除
      opa->clear_output();
      // nop.outputs 追加
      nop->append_output(value);
      // value.source 更新
      value->set_source(nop);

      // connect(opa, nop)
      // new_value.source, new_value.target 確定
      auto new_value = std::make_shared<Value>(opa, nop);
      // opa.output 確定
      opa->append_output(new_value);
      // nop.input 確定
      nop->append_input(std::move(new_value));
      assert(nop->inputs().back().use_count() == 2);

      // この時点のグラフ:
      // ... -> opa -(new_value)-> nop -(value)-> next_node -> ...
    }

    Graph graph_{};
  };
} // namespace ns
