/// @file lex.hpp
#pragma once
#include <cassert>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#define NS_RESULT_TRY_IMPL(var, result)                                        \
  auto&& ns_tmp_##var = (result);                                              \
  if (ns_tmp_##var.index() == 1)                                               \
    return std::get<1>(std::forward<decltype(ns_tmp_##var)>(ns_tmp_##var));    \
  [[maybe_unused]] auto&& var =                                                \
    std::get<0>(std::forward<decltype(ns_tmp_##var)>(ns_tmp_##var))

#define NS_RESULT_TRY(var, result) NS_RESULT_TRY_IMPL(var, result)

namespace ns {
  struct Error {
    std::string_view msg;
    friend bool operator==(const Error&, const Error&) = default;
  };
} // namespace ns
