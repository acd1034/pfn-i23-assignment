/// @file lex.hpp
#pragma once
#include <cassert>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#define NS_RESULT_TRY_IMPL(var, result)                                \
  auto&& ns_temporary##var = (result);                                 \
  if (ns_temporary##var.index() == 1)                                  \
    return std::get<1>(                                                \
        std::forward<decltype(ns_temporary##var)>(ns_temporary##var)); \
  [[maybe_unused]] auto&& var = std::get<0>(                           \
      std::forward<decltype(ns_temporary##var)>(ns_temporary##var))

#define NS_RESULT_TRY(var, result) NS_RESULT_TRY_IMPL(var, result)

namespace ns {
  struct Error {
    std::string_view msg;
    friend bool operator==(const Error&, const Error&) = default;
  };
} // namespace ns
