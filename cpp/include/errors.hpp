// errors.hpp
// Declares/Implements: ErrorKind, Error

#pragma once

#include <format>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

#include "ast_common.hpp"

enum class ErrorKind {
    Runtime, Syntax, Type, Arity, DivideByZero
};

inline const char* to_string(ErrorKind kind) {
    static constexpr const char* names[] = {
        "Runtime", "Syntax", "Type", "Arity", "DivideByZero"
    };
    return names[static_cast<size_t>(kind)];
}

class Error {
    std::string message_;
    std::optional<Span> span_;
    ErrorKind kind_;

    std::string fmt_span() const {
        if (!span_) return {};
        return std::format("at line {}, col {}", span_->p1.line, span_->p1.column);
    }

public:
    Error(std::string message, ErrorKind kind = ErrorKind::Runtime)
        : message_(std::move(message)), kind_(kind) {} 

    Error(std::string message, const Span& span, ErrorKind kind = ErrorKind::Runtime)
        : message_(std::move(message)), kind_(kind), span_(span) {}

    const std::string& what() const noexcept { return message_; }
    ErrorKind kind() const noexcept { return kind_; }
    std::string kind_name() const noexcept { return to_string(kind_); }
    const std::optional<Span>& span() const noexcept { return span_; }

    std::string fmt() const {
        auto span_str = fmt_span();
        if (span_str.empty()) {
            return std::format("{} Error: {}", to_string(kind_), message_);
        }
        return std::format("{} Error {}: {}", to_string(kind_), span_str, message_);
    }
};