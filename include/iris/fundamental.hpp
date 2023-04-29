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

#define NS_RESULT_AUTO_IMPL(v, r)                                              \
  auto&& ns_tmp_##v = (r);                                                     \
  if (ns_tmp_##v.index() == 1)                                                 \
    return std::get<1>(std::forward<decltype(ns_tmp_##v)>(ns_tmp_##v));        \
  [[maybe_unused]] auto&& v =                                                  \
    std::get<0>(std::forward<decltype(ns_tmp_##v)>(ns_tmp_##v))

#define NS_RESULT_AUTO(v, r) NS_RESULT_AUTO_IMPL(v, r)

namespace ns {
  struct Error {
    std::string_view msg;
    friend bool operator==(const Error&, const Error&) = default;
  };
} // namespace ns
