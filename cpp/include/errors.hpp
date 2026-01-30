// errors.h

#pragma once

#include <array>
#include <optional>
#include <string>
#include <sstream>
#include <utility>

#include "ast.h"

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
        if (!span_.has_value()) return std::string();
        std::stringstream ss;
        ss << "at line " << span_->p1.line << ", col " << span_->p1.column;
        return ss.str();
    }

public:
    Error(std::string message, ErrorKind kind = ErrorKind::Runtime)
        : message_(std::move(message)), kind_(kind), span_(std::nullopt) {}

    Error(std::string message, const Span& span, ErrorKind kind = ErrorKind::Runtime)
        : message_(std::move(message)), kind_(kind), span_(span) {}

    virtual ~Error() = default;

    const std::string& what() const noexcept { return message_; }
    ErrorKind kind() const noexcept { return kind_; }
    std::string kind_name() const noexcept { return to_string(kind_); }
    const std::optional<Span>& span() const noexcept { return span_; }

    virtual std::string fmt() const {
        std::stringstream ss;
        ss << to_string(kind_) << " Error";
        auto s = fmt_span();
        if (!s.empty()) ss << " " << s;
        ss << ": " << message_;
        return ss.str();
    }
};