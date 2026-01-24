// utils.hpp

#pragma once

#include <memory>
#include <optional>
#include <vector>

namespace utils {

/// @brief Deep-copy of a std::unqiue_ptr
/// @tparam T Copy-constructible type
/// @param src Source pointer
/// @return new unique_ptr owning a copy of *src or nullptr if src is null
template <typename T>
std::unique_ptr<T> clone(const std::unique_ptr<T>& src) {
    return src ? std::make_unique<T>(*src) : nullptr;
}

/// @brief Deep copy of a std::optional
/// @tparam T Copy-constructible type
/// @param src Source pointer
/// @return new std::optional
template <typename T>
std::optional<T> clone(const std::optional<T>& src) {
    if (!src) return std::nullopt;
    return clone(*src);
}

/// @brief Deep-copy of a vector of std::unique_ptr
/// @tparam T Copy-constructible type
/// @param src Source vector
/// @return Vector owning deep copies of all elements in src.
template <typename T>
std::vector<std::unique_ptr<T>> clone(const std::vector<std::unique_ptr<T>>& src) {
    std::vector<std::unique_ptr<T>> dst;
    dst.reserve(src.size());
    for (const auto& item : src) {
        dst.push_back(clone(item));
    }
    return dst;
}
}