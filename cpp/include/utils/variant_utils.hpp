// variant_utils.hpp
// Declares/Implements: overloaded

#pragma once

#include <variant>

/// @brief helper for variant visit
/// @tparam ...Ts
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
