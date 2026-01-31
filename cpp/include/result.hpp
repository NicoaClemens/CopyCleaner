// result.hpp
// Declares/Implements: Result, ok, err, is_ok, is_err

#pragma once

#include <cassert>
#include <memory>
#include <type_traits>
#include <variant>

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

    /// @brief Checks if the Result contains a valid value (not an error)
    /// @return true if Result holds a value, false if it holds an error
    bool is_ok() const noexcept {
        return std::holds_alternative<T>(data_);
    }
    /// @brief Checks if the Result contains an error (not a valid value)
    /// @return true if Result holds an error, false if it holds a value
    bool is_err() const noexcept {
        return std::holds_alternative<std::shared_ptr<Error>>(data_);
    }

    /// @brief gets Value. Asserts Result is ok first
    T& value() & {
        assert(is_ok());
        return std::get<T>(data_);
    }

    /// @brief gets Value reference. Asserts Result is ok first
    const T& value() const& {
        assert(is_ok());
        return std::get<T>(data_);
    }

    /// @brief gets Value. Asserts Result is ok first
    T&& value() && {
        assert(is_ok());
        return std::get<T>(std::move(data_));
    }

    /// @brief Gets the error from the Result. Assumes Result contains an error
    /// @return Reference to the shared_ptr containing the Error
    std::shared_ptr<Error>& error() & {
        return std::get<std::shared_ptr<Error>>(data_);
    }
    /// @brief Gets the error from the Result (const version). Assumes Result contains an error
    /// @return Const reference to the shared_ptr containing the Error
    const std::shared_ptr<Error>& error() const& {
        return std::get<std::shared_ptr<Error>>(data_);
    }

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
inline bool is_ok(const Result<T>& r) noexcept {
    return r.is_ok();
}

template <typename T>
inline bool is_err(const Result<T>& r) noexcept {
    return r.is_err();
}