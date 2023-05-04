/// @file graph.hpp
#pragma once
#include <algorithm> // std::find_if
#include <iosfwd>
#include "fundamental.hpp"

namespace ns {
  class Node;

  class Value {
  public:
    Value() = default;
    Value(std::shared_ptr<Node> source, std::shared_ptr<Node> target)
      : source_(std::move(source)), target_(std::move(target)) {}

    std::shared_ptr<Node> source() const { return source_.lock(); }
    std::shared_ptr<Node> target() const { return target_.lock(); }

    void set_source(std::shared_ptr<Node> node) { source_ = std::move(node); }
    void set_target(std::shared_ptr<Node> node) { target_ = std::move(node); }

  private:
    std::weak_ptr<Node> source_{};
    std::weak_ptr<Node> target_{};
  };

  class Node {
  public:
    Node() = default;
    Node(std::size_t id, std::string name, std::size_t memory_usage = 1)
      : id_(std::move(id)), name_(std::move(name)),
        memory_usage_(memory_usage) {}

    std::size_t id() const { return id_; }
    std::string_view name() const { return name_; }
    std::size_t memory_usage() const { return memory_usage_; }
    const std::vector<std::shared_ptr<Value>>& inputs() const {
      return inputs_;
    }
    const std::vector<std::shared_ptr<Value>>& outputs() const {
      return outputs_;
    }

    void append_input(std::shared_ptr<Value> value) {
      inputs_.push_back(std::move(value));
    }
    void append_output(std::shared_ptr<Value> value) {
      outputs_.push_back(std::move(value));
    }

    void erase_input(const std::shared_ptr<Value>& value) {
      auto pred = [&value](const std::shared_ptr<Value>& input) {
        return input->source()->id() == value->source()->id();
      };
      auto it = std::find_if(inputs_.begin(), inputs_.end(), std::move(pred));
      assert(it != inputs_.end());
      inputs_.erase(std::move(it));
    }
    void erase_output(const std::shared_ptr<Value>& value) {
      auto pred = [&value](const std::shared_ptr<Value>& output) {
        return output->target()->id() == value->target()->id();
      };
      auto it = std::find_if(outputs_.begin(), outputs_.end(), std::move(pred));
      assert(it != outputs_.end());
      outputs_.erase(std::move(it));
    }

  private:
    std::size_t id_{};
    std::string name_{};
    std::size_t memory_usage_{};
    std::vector<std::shared_ptr<Value>> inputs_{};
    std::vector<std::shared_ptr<Value>> outputs_{};
  };

  void connect_nodes(std::shared_ptr<Node> from, std::shared_ptr<Node> to) {
    // new_value.source, new_value.target 確定
    auto new_value = std::make_shared<Value>(from, to);
    // from.outputs 追加
    from->append_output(new_value);
    // to.inputs 追加
    to->append_input(new_value);

    assert(new_value.use_count() == 3);
  }

  void disconnect_nodes(std::shared_ptr<Value> value) {
    [[maybe_unused]] long use_count = value.use_count();
    std::shared_ptr<Node> from = value->source();
    std::shared_ptr<Node> to = value->target();

    // from.outputs から value 削除
    from->erase_output(value);
    // to.inputs から value 削除
    to->erase_input(value);

    assert(value.use_count() == use_count - 2);
  }

  /// value.source を to に変更する
  void relink_source_node(std::shared_ptr<Value> value,
                          std::shared_ptr<Node> to) {
    [[maybe_unused]] long use_count = value.use_count();
    std::shared_ptr<Node> from = value->source();

    // from.outputs から value 削除
    from->erase_output(value);
    // to.outputs 追加
    to->append_output(value);
    // value.source 更新
    value->set_source(to);

    assert(value.use_count() == use_count);
  }

  class Graph {
  public:
    using node_iterator = std::vector<std::shared_ptr<Node>>::const_iterator;

    Graph() = default;

    const std::vector<std::shared_ptr<Node>>& nodes() const { return nodes_; }

    std::size_t unique_id() { return count_++; }

    node_iterator insert_node(node_iterator pos, std::shared_ptr<Node> node) {
      return nodes_.insert(pos, std::move(node));
    }

    node_iterator erase_node(node_iterator pos) {
      assert((*pos)->inputs().size() == 0 and (*pos)->outputs().size() == 0);
      return nodes_.erase(pos);
    }

  private:
    std::vector<std::shared_ptr<Node>> nodes_{};
    std::size_t count_{};
  };

  std::ostream& operator<<(std::ostream& os,
                           const std::shared_ptr<Node>& node) {
    os << "%" << node->id() << " = " << node->name() << "(";
    const char* dlm = "";
    for (std::shared_ptr<Value> value : node->inputs()) {
      os << std::exchange(dlm, ", ") << "%" << value->source()->id();
    }
    return os << ")";
  }

  std::ostream& operator<<(std::ostream& os, const Graph& graph) {
    const char* dlm = "";
    for (std::shared_ptr<Node> node : graph.nodes()) {
      os << std::exchange(dlm, "\n") << node;
    }
    return os;
  }

  class GraphBuilder {
  private:
    Graph graph_{};
    Graph::node_iterator insert_point_ = graph_.nodes().end();

  public:
    GraphBuilder() = default;
    explicit GraphBuilder(Graph graph)
      : graph_(std::move(graph)), insert_point_(graph_.nodes().end()) {}

    const Graph& graph() const& { return graph_; }
    Graph graph() && { return std::move(graph_); }
    Graph::node_iterator insert_point() const { return insert_point_; }

    void set_insert_point(Graph::node_iterator pos) { insert_point_ = pos; }

    std::shared_ptr<Node> build_node(std::string name,
                                     std::vector<std::shared_ptr<Node>> args) {
      auto next_node =
        std::make_shared<Node>(graph_.unique_id(), std::move(name));
      insert_point_ = graph_.insert_node(std::move(insert_point_), next_node);
      ++insert_point_;
      for (auto&& prev_node : args) {
        // ... -> prev_node
        //        next_node
        connect_nodes(std::move(prev_node), next_node);
        // ... -> prev_node -> next_node
      }
      assert(next_node.use_count() == 2);
      return next_node;
    }
  };
} // namespace ns
