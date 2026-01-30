// result.hpp
#pragma once
#include <memory>
#include <variant>
#include <type_traits>

#include "errors.hpp"

template <typename T>
using Result = std::variant<T, std::shared_ptr<Error>>;

template <typename T>
inline Result<T> ok(T value) {
	return Result<T>(std::in_place_index<0>, std::move(value));
}

template <typename T>
inline Result<T> err(std::shared_ptr<Error> e) {
	return Result<T>(std::in_place_index<1>, std::move(e));
}

template <typename T>
inline bool is_ok(const Result<T>& r) noexcept { return std::holds_alternative<T>(r); }

template <typename T>
inline bool is_err(const Result<T>& r) noexcept { return std::holds_alternative<std::shared_ptr<Error>>(r); }