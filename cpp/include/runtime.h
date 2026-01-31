// runtime.h

#pragma once

#include <memory>
#include <optional>
#include <map>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "ast.h"
#include "errors.hpp"
#include "result.hpp"
#include "runtime_value.h"
#include "variant_utils.hpp"


struct ExecFlow {
    struct None {};
    struct Return { RuntimeValue value; };
    struct Break {};
    struct Continue {};

    using Value = std::variant<None, Return, Break, Continue>;

    Value value;
};

struct Environment;
using envPtr = std::shared_ptr<Environment>;
struct Environment : public std::enable_shared_from_this<Environment> {
    std::unordered_map<std::string, RuntimeValue> variables;
    envPtr parent = nullptr;

    Environment() { variables = std::unordered_map<std::string, RuntimeValue>(); };
    Environment(envPtr _parent) : parent(_parent) { variables = std::unordered_map<std::string, RuntimeValue>(); };
    
    std::optional<RuntimeValue> get(const std::string& name);
    void set(const std::string& name, const RuntimeValue& value);
};

struct MethodRepr {
    std::vector<std::pair<std::string, AstType>> args;
    AstType returnType = astCreateNull();
    std::vector<Statement> body;
};

struct Interpreter {
    envPtr global_env = std::make_shared<Environment>();
    std::unordered_map<std::string, MethodRepr> functions;

    Result<RuntimeValue> run(std::vector<Statement>& stmts);
    Result<ExecFlow> eval_statements(std::vector<Statement>& stmts, Environment& env);
    Result<RuntimeValue> eval_expr(Expr& expr, envPtr env);
};