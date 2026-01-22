// runtime.cpp

#include "runtime.h"

std::optional<RuntimeValue> Environment::get(const std::string& name) {
    if (variables.contains(name)) return variables[name];
}

void Environment::set(const std::string& name, const RuntimeValue& value) {
    variables[name] = value;
}

Result<RuntimeValue> Interpreter::run(std::vector<Statement>& stmts) {
    Result<ExecFlow> r = this->eval_statements(stmts, this->global_env);
    if (r.success == ResultType::Error) return Result<RuntimeValue>{ RuntimeValue{}, r.success, r.error }
    
}