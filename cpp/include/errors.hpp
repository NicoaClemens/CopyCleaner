// errors.h

#pragma once

#include <optional>
#include <string>
#include <sstream>

#include "ast.h"

struct Error {
    std::string message;
    Span* span;
    Error() : message(), span(nullptr) {}
    Error(std::string m, Span* s = nullptr) : message(std::move(m)), span(s) {}
    virtual ~Error() = default;
    virtual std::string fmt() const = 0;
};

struct RuntimeError : public Error {
    using Error::Error;
    std::string fmt() const override {
        std::stringstream ss;
        if (span)
            ss << "Runtime Error at line " << span->p1.line << ", pos " << span->p1.column << ": " << message;
        else
            ss << "Runtime Error: " << message;
        return ss.str();
    }
};
struct SyntaxError  : public Error {
    using Error::Error;
    std::string fmt() const override {
        std::stringstream ss;
        if (span)
            ss << "Syntax Error at line " << span->p1.line << ", pos " << span->p1.column << ": " << message;
        else
            ss << "Syntax Error: " << message;
        return ss.str();
    }
};

enum class ResultType{
    OK,
    Error,
};


/// @brief `error` must be set if `success` is `ResultType::Error` 
template <typename T> 
struct Result {
    T value;
    ResultType success;
    std::optional<std::shared_ptr<Error>> error; 
};