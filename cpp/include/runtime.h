// runtime.h
// Declares: ExecFlow, Environment, MethodRepr, Interpreter

#pragma once

#include <map>
#include <memory>
#include <optional>
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
    struct Return {
        RuntimeValue value;
    };
    struct Break {};
    struct Continue {};
    struct Exit {};

    using Value = std::variant<None, Return, Break, Continue, Exit>;

    Value value;
};

struct Environment;
using env_ptr = std::shared_ptr<Environment>;
struct Environment : public std::enable_shared_from_this<Environment> {
    std::unordered_map<std::string, RuntimeValue> variables;
    env_ptr parent = nullptr;

    Environment() {
        variables = std::unordered_map<std::string, RuntimeValue>();
    };
    Environment(env_ptr _parent) : parent(_parent) {
        variables = std::unordered_map<std::string, RuntimeValue>();
    };

    /// @brief Retrieves a variable by name, checking parent scopes if not found locally
    /// @param name The variable name to look up
    /// @return Optional containing the RuntimeValue if found, or nullopt if not found in any scope
    std::optional<RuntimeValue> get(const std::string& name);
    /// @brief Sets a variable in the current environment scope
    /// @param name The variable name to set
    /// @param value The RuntimeValue to assign to the variable
    void set(const std::string& name, const RuntimeValue& value);
};

struct MethodRepr {
    std::vector<std::pair<std::string, AstType>> args;
    AstType returnType = astCreateNull();
    std::vector<Statement> body;
};

struct Interpreter {
    env_ptr global_env = std::make_shared<Environment>();
    std::unordered_map<std::string, MethodRepr> functions;

    /// @brief Executes a list of statements and returns the final result
    /// @param stmts Vector of statements to execute
    /// @return Result containing the final RuntimeValue, or an error if execution failed
    Result<RuntimeValue> run(const std::vector<Statement>& stmts);
    /// @brief Evaluates a sequence of statements in a given environment, handling control flow
    /// @param stmts Vector of statements to evaluate
    /// @param env The environment to execute in (handles variable scoping)
    /// @return Result containing ExecFlow indicating normal completion, return, break, or continue
    Result<ExecFlow> eval_statements(const std::vector<Statement>& stmts, Environment& env);
    Result<ExecFlow> eval_statements(const std::vector<StmtPtr>& stmts, Environment& env);
    /// @brief Evaluates a single expression to produce a RuntimeValue
    /// @param expr The expression to evaluate
    /// @param env The environment to evaluate in (for variable lookups and scoping)
    /// @return Result containing the computed RuntimeValue, or an error if evaluation failed
    Result<RuntimeValue> eval_expr(const Expr& expr, env_ptr env);
};