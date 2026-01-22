// runtime.h

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "ast.h"
#include "errors.hpp"
#include "runtime_value.h"


struct ExecFlow {
    struct None {};
    struct Return { RuntimeValue value; };
    struct Break {};
    struct Continue {};

    using Value = std::variant<None, Return, Break, Continue>;

    Value value;
};

struct Environment;
using envPtr = std::unique_ptr<Environment>;
struct Environment {
    std::unordered_map<std::string, RuntimeValue> variables;
    envPtr parent = nullptr;

    Environment() { variables = std::unordered_map<std::string, RuntimeValue>(); };
    Environment(envPtr _parent) : parent(std::move(_parent)) { variables = std::unordered_map<std::string, RuntimeValue>(); };
    
    std::optional<RuntimeValue> get(const std::string& name);
    void set(const std::string& name, const RuntimeValue& value);
};

struct MethodRepr {
    std::unordered_map<std::string, AstType> args;
    AstType returnType;
    std::vector<Statement> body;
};

struct Interpreter {
    Environment global_env;
    std::unordered_map<std::string, MethodRepr> functions;

    Result<RuntimeValue> run(std::vector<Statement>& stmts);
    Result<ExecFlow> eval_statements(std::vector<Statement>& stmts, Environment& env);
    Result<RuntimeValue> eval_expr(Expr& expr, envPtr env);
};