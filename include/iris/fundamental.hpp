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

namespace ns {
  struct Error {
    std::string_view msg;
    friend bool operator==(const Error&, const Error&) = default;
  };
}
