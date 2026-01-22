// errors.h

#pragma once

#include <string>
#include <sstream>

#include "ast.h"

struct Error {
    std::string message;
    Span* span;
    virtual std::string fmt() const = 0;
};

struct RuntimeError : public Error {
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

template <typename T>
struct Result {
    T value;
    ResultType success;
    Error error;
};