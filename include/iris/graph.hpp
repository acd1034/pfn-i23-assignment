/// @file graph.hpp
#pragma once
#include <iosfwd>
#include "fundamental.hpp"

namespace ns {
  class Value;
  class GraphBuilder;

  class Node {
  public:
    Node() = default;
    Node(std::size_t id, std::string name)
      : id_(std::move(id)), name_(std::move(name)) {}

    std::size_t id() const { return id_; }
    std::string_view name() const { return name_; }
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

    void clear_input() { inputs_.clear(); }
    void clear_output() { outputs_.clear(); }

  private:
    std::size_t id_{};
    std::string name_{};
    std::vector<std::shared_ptr<Value>> inputs_{};
    std::vector<std::shared_ptr<Value>> outputs_{};
  };

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

  class Graph {
  public:
    using insert_point_t = std::vector<std::shared_ptr<Node>>::const_iterator;

    Graph() = default;

    const std::vector<std::shared_ptr<Node>>& nodes() const { return nodes_; }

    std::size_t unique_id() { return count_++; }

    insert_point_t insert_node(insert_point_t pos, std::shared_ptr<Node> node) {
      return nodes_.insert(pos, std::move(node));
    }

    insert_point_t erase_node(insert_point_t pos) {
      assert((*pos)->inputs().size() == 0 and (*pos)->outputs().size() == 0);
      return nodes_.erase(pos);
    }

  private:
    std::vector<std::shared_ptr<Node>> nodes_{};
    std::size_t count_{};
  };

  template <class CharT, class Traits>
  void print_node(std::basic_ostream<CharT, Traits>& os,
                  std::shared_ptr<Node> node) {
    os << "%" << node->id() << " = " << node->name() << "(";
    const char* dlm = "";
    for (std::shared_ptr<Value> value : node->inputs()) {
      std::shared_ptr<Node> source = value->source();
      os << std::exchange(dlm, ", ") << "%" << source->id();
    }
    os << ")";
  }

  template <class CharT, class Traits>
  std::basic_ostream<CharT, Traits>&
  operator<<(std::basic_ostream<CharT, Traits>& os, const Graph& graph) {
    const char* dlm = "";
    for (std::shared_ptr<Node> node : graph.nodes()) {
      os << std::exchange(dlm, "\n");
      print_node(os, node);
    }
    return os;
  }

  class GraphBuilder {
  private:
    Graph graph_{};
    Graph::insert_point_t insert_point_ = graph_.nodes().end();

  public:
    GraphBuilder() = default;
    explicit GraphBuilder(Graph graph)
      : graph_(std::move(graph)), insert_point_(graph_.nodes().end()) {}

    const Graph& graph() const& { return graph_; }
    Graph graph() && { return std::move(graph_); }
    Graph::insert_point_t insert_point() const { return insert_point_; }

    void set_insert_point(Graph::insert_point_t pos) { insert_point_ = pos; }

    std::shared_ptr<Node> build_node(std::string name,
                                     std::vector<std::shared_ptr<Node>> args) {
      // ... -> prev_node -(value)-> next_node
      auto next_node =
        std::make_shared<Node>(graph_.unique_id(), std::move(name));
      for (auto&& prev_node : args) {
        // value.source, value.target 確定
        auto value = std::make_shared<Value>(prev_node, next_node);
        // prev_node.output 確定
        prev_node->append_output(value);
        // next_node.input 確定
        next_node->append_input(std::move(value));
        assert(next_node->inputs().back().use_count() == 2);
      }
      insert_point_ = graph_.insert_node(std::move(insert_point_), next_node);
      assert(next_node.use_count() == 2);
      ++insert_point_;
      return next_node;
    }
  };
} // namespace ns
