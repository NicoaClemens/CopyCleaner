// result.hpp
#pragma once
#include <memory>
#include <variant>
#include <type_traits>
#include <cassert>

#include "errors.hpp"

template <typename T>
class Result {
public:
	using value_type = T;

	Result() = delete;
	Result(const Result&) = default;
	Result(Result&&) = default;
	Result& operator=(const Result&) = default;
	Result& operator=(Result&&) = default;

	explicit Result(T v) : data_(std::move(v)) {}
	explicit Result(std::shared_ptr<Error> e) : data_(std::move(e)) {}

	bool is_ok() const noexcept { return std::holds_alternative<T>(data_); }
	bool is_err() const noexcept { return std::holds_alternative<std::shared_ptr<Error>>(data_); }

	T& value() & { assert(is_ok()); return std::get<T>(data_); }
	const T& value() const & { assert(is_ok()); return std::get<T>(data_); }
	T&& value() && { assert(is_ok()); return std::get<T>(std::move(data_)); }

	std::shared_ptr<Error>& error() & { return std::get<std::shared_ptr<Error>>(data_); }
	const std::shared_ptr<Error>& error() const & { return std::get<std::shared_ptr<Error>>(data_); }

private:
	std::variant<T, std::shared_ptr<Error>> data_;
};

template <typename T>
inline Result<T> ok(T value) {
	return Result<T>(std::move(value));
}

template <typename T>
inline Result<T> err(std::shared_ptr<Error> e) {
	return Result<T>(std::move(e));
}

template <typename T>
inline bool is_ok(const Result<T>& r) noexcept { return r.is_ok(); }

template <typename T>
inline bool is_err(const Result<T>& r) noexcept { return r.is_err(); }