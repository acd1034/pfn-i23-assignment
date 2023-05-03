/// @file memory_leak_analyzer.hpp
#pragma once
#include <algorithm> // std::transform, std::all_of
#include "fundamental.hpp"
#include "graph.hpp"

namespace ns {
  struct MemoryLeakStatistics {
    Graph::insert_point_t pos;
    long node_use_count;
    std::vector<long> outputs_use_count;
  };

  struct MemoryLeakAnalyzer {
    explicit MemoryLeakAnalyzer(const Graph& graph) : graph_(graph) {}
    MemoryLeakAnalyzer(Graph&& graph) = delete;

    MemoryLeakStatistics run() && {
      stat_.pos = graph_.nodes().end();

      auto it = graph_.nodes().begin();
      while (it != graph_.nodes().end()) {
        it = run_on_node(std::move(it));
      }
      return std::move(stat_);
    }

    Graph::insert_point_t run_on_node(Graph::insert_point_t it) {
      long node_use_count = it->use_count();
      std::vector<long> outputs_use_count((*it)->outputs().size());
      std::transform(
        (*it)->outputs().begin(), (*it)->outputs().end(),
        outputs_use_count.begin(),
        [](const std::shared_ptr<Value>& value) { return value.use_count(); });
      bool valid =
        node_use_count == 1
        and std::all_of(outputs_use_count.begin(), outputs_use_count.end(),
                        [](long use_count) { return use_count == 2; });

      if (valid) {
        ++it;
        return it;
      }

      stat_ = MemoryLeakStatistics{std::move(it), std::move(node_use_count),
                                   std::move(outputs_use_count)};
      it = graph_.nodes().end();
      return it;
    }

  private:
    const Graph& graph_;
    MemoryLeakStatistics stat_{};
  };
} // namespace ns
