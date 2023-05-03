/// @file memory_usage_analyzer.hpp
#pragma once
#include <numeric> // std::iota
#include <unordered_map>
#include "fundamental.hpp"
#include "graph.hpp"

namespace ns {
  struct MemoryUsageStatistics {
    std::vector<std::size_t> last_uses;
    std::vector<std::size_t> usages;
  };

  struct MemoryUsageAnalyzer {
    explicit MemoryUsageAnalyzer(const Graph& graph)
      : graph_(graph), stat_{std::vector<std::size_t>(graph_.nodes().size()),
                             std::vector<std::size_t>(graph_.nodes().size())} {}
    MemoryUsageAnalyzer(Graph&& graph) = delete;

    MemoryUsageStatistics run() && {
      for (std::size_t i = 0; i < graph_.nodes().size(); ++i) {
        find_order_by_id_[graph_.nodes()[i]->id()] = i;
      }

      std::iota(stat_.last_uses.begin(), stat_.last_uses.end(), 0);

      for (std::size_t i = 0; i < graph_.nodes().size(); ++i) {
        for (const auto& value : graph_.nodes()[i]->inputs()) {
          std::size_t order = find_order_by_id_[value->source()->id()];
          stat_.last_uses[order] = i;
        }
      }

      for (std::size_t i = 0; i < stat_.last_uses.size(); ++i) {
        for (std::size_t j = i; j <= stat_.last_uses[i]; ++j) {
          ++stat_.usages[j];
        }
      }
      return std::move(stat_);
    }

  private:
    const Graph& graph_;
    std::unordered_map<std::size_t, std::size_t> find_order_by_id_{};
    MemoryUsageStatistics stat_{};
  };
} // namespace ns
