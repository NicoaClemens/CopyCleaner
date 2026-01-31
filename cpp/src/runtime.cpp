// runtime.cpp
// Implements runtime.h

#include "runtime.h"

#include <cmath>
#include <cstdlib>

#include "runtime_utils.h"
#include "types_utils.hpp"

std::optional<RuntimeValue> Environment::get(const std::string& name) {
    if (variables.contains(name)) return variables[name];
    if (parent) return parent->get(name);
    return std::nullopt;
}

void Environment::set(const std::string& name, const RuntimeValue& value) {
    variables[name] = value;
}

Result<RuntimeValue> Interpreter::run(std::vector<Statement>& stmts) {
    auto r = this->eval_statements(stmts, *this->global_env);
    if (is_err(r)) return err<RuntimeValue>(r.error());
    ExecFlow exec = r.value();
    return std::visit(overloaded{[](ExecFlow::None) -> Result<RuntimeValue> {
                                     return ok(RuntimeValue{RuntimeValue::Null{}});
                                 },
                                 [](ExecFlow::Return& r) -> Result<RuntimeValue> {
                                     return ok(RuntimeValue{r.value});
                                 },
                                 [](ExecFlow::Break) -> Result<RuntimeValue> {
                                     return err<RuntimeValue>(std::make_shared<Error>(
                                         "invalid 'break' statement", ErrorKind::Syntax));
                                 },
                                 [](ExecFlow::Continue) -> Result<RuntimeValue> {
                                     return err<RuntimeValue>(std::make_shared<Error>(
                                         "invalid 'continue' statement", ErrorKind::Syntax));
                                 }},
                      exec.value);
}

Result<ExecFlow> Interpreter::eval_statements(std::vector<Statement>& stmts, Environment& env) {
    for (auto& s : stmts) {
        // Assignment
        if (auto a = std::get_if<Statement::Assignment>(&s.value)) {
            auto r = this->eval_expr(a->expr, env.shared_from_this());
            if (is_err(r)) return err<ExecFlow>(r.error());
            env.set(a->name, r.value());
            continue;
        }

        // If
        if (auto i = std::get_if<Statement::If>(&s.value)) {
            auto cond_r = this->eval_expr(i->condition, env.shared_from_this());
            if (is_err(cond_r)) return err<ExecFlow>(cond_r.error());
            if (is_truthy(cond_r.value())) {
                auto child = std::make_shared<Environment>(env.shared_from_this());
                std::vector<Statement> body_vals;
                for (auto& sp : i->body) body_vals.push_back(*sp);
                auto flow = this->eval_statements(body_vals, *child);
                if (is_err(flow)) return err<ExecFlow>(flow.error());
                if (!std::holds_alternative<ExecFlow::None>(flow.value().value)) return flow;
            } else {
                bool matched = false;
                for (auto& el : i->elif) {
                    auto er = this->eval_expr(el.first, env.shared_from_this());
                    if (is_err(er)) return err<ExecFlow>(er.error());
                    if (is_truthy(er.value())) {
                        matched = true;
                        auto child = std::make_shared<Environment>(env.shared_from_this());
                        std::vector<Statement> body_vals;
                        for (auto& sp : el.second) body_vals.push_back(*sp);
                        auto flow = this->eval_statements(body_vals, *child);
                        if (is_err(flow)) return err<ExecFlow>(flow.error());
                        if (!std::holds_alternative<ExecFlow::None>(flow.value().value))
                            return flow;
                        break;
                    }
                }
                if (!matched) {
                    auto child = std::make_shared<Environment>(env.shared_from_this());
                    std::vector<Statement> body_vals;
                    for (auto& sp : i->else_body) body_vals.push_back(*sp);
                    auto flow = this->eval_statements(body_vals, *child);
                    if (is_err(flow)) return err<ExecFlow>(flow.error());
                    if (!std::holds_alternative<ExecFlow::None>(flow.value().value)) return flow;
                }
            }
            continue;
        }

        // While
        if (auto w = std::get_if<Statement::While>(&s.value)) {
            while (true) {
                auto cr = this->eval_expr(w->condition, env.shared_from_this());
                if (is_err(cr)) return err<ExecFlow>(cr.error());
                if (!is_truthy(cr.value())) break;
                auto child = std::make_shared<Environment>(env.shared_from_this());
                std::vector<Statement> body_vals;
                for (auto& sp : w->body) body_vals.push_back(*sp);
                auto flow = this->eval_statements(body_vals, *child);
                if (is_err(flow)) return err<ExecFlow>(flow.error());
                if (std::holds_alternative<ExecFlow::Return>(flow.value().value)) return flow;
                if (std::holds_alternative<ExecFlow::Break>(flow.value().value)) break;
                if (std::holds_alternative<ExecFlow::Continue>(flow.value().value)) continue;
            }
            continue;
        }

        // Return
        if (auto r = std::get_if<Statement::Return>(&s.value)) {
            auto vr = this->eval_expr(r->value, env.shared_from_this());
            if (is_err(vr)) return err<ExecFlow>(vr.error());
            ExecFlow out;
            out.value = ExecFlow::Return{vr.value()};
            return ok(out);
        }

        // FunctionDef
        if (auto f = std::get_if<Statement::FunctionDef>(&s.value)) {
            MethodRepr m;
            for (auto& p : f->params) m.args.emplace_back(p.first, p.second);
            m.returnType = f->return_type.value_or(AstType());
            for (auto& sp : f->body) m.body.push_back(*sp);
            this->functions[f->name] = std::move(m);
            continue;
        }

        // Break
        if (std::holds_alternative<Statement::Break>(s.value)) {
            ExecFlow out;
            out.value = ExecFlow::Break{};
            return ok(out);
        }

        // Continue
        if (std::holds_alternative<Statement::Continue>(s.value)) {
            ExecFlow out;
            out.value = ExecFlow::Continue{};
            return ok(out);
        }
    }

    ExecFlow none;
    none.value = ExecFlow::None{};
    return ok(none);
}

Result<RuntimeValue> Interpreter::eval_expr(Expr& expr, envPtr env) {
    using E = Expr;

    auto make_err = [&](std::shared_ptr<Error> e) -> Result<RuntimeValue> {
        return err<RuntimeValue>(e);
    };

    if (std::holds_alternative<E::Literal>(expr.value)) {
        auto lit = std::get<E::Literal>(expr.value);
        return ok(lit.value);
    }

    if (std::holds_alternative<E::Variable>(expr.value)) {
        auto v = std::get<E::Variable>(expr.value);
        if (!env) return ok(RuntimeValue{RuntimeValue::Null{}});
        auto ov = env->get(v.name);
        if (ov.has_value()) return ok(ov.value());
        return ok(RuntimeValue{RuntimeValue::Null{}});
    }

    if (std::holds_alternative<E::UnaryOp>(expr.value)) {
        const auto& u = std::get<E::UnaryOp>(expr.value);
        auto r = this->eval_expr(*u.next, env);
        if (is_err(r)) return make_err(r.error());

        if (u.op == Operator::Not) {
            bool val = is_truthy(r.value());
            RuntimeValue out;
            out.value = RuntimeValue::Bool{!val};
            return ok(out);
        }
        if (u.op == Operator::Neg) {
            // numeric negation
            if (std::holds_alternative<RuntimeValue::Int>(r.value().value)) {
                auto v = std::get<RuntimeValue::Int>(r.value().value).value;
                RuntimeValue out;
                out.value = RuntimeValue::Int{-v};
                return ok(out);
            }
            if (std::holds_alternative<RuntimeValue::Float>(r.value().value)) {
                auto v = std::get<RuntimeValue::Float>(r.value().value).value;
                RuntimeValue out;
                out.value = RuntimeValue::Float{-v};
                return ok(out);
            }
            return err<RuntimeValue>(
                std::make_shared<Error>("unsupported operand type for unary -", ErrorKind::Type));
        }
        return ok(RuntimeValue{RuntimeValue::Null{}});
    }

    if (std::holds_alternative<E::BinaryOp>(expr.value)) {
        const auto& b = std::get<E::BinaryOp>(expr.value);
        auto lr = this->eval_expr(*b.left, env);
        if (is_err(lr)) return make_err(lr.error());
        RuntimeValue l = lr.value();

        // Short-circuiting logical operators implemented here so RHS isn't evaluated unnecessarily
        switch (b.op) {
            case Operator::And: {
                if (!is_truthy(l)) {
                    RuntimeValue out;
                    out.value = RuntimeValue::Bool{false};
                    return ok(out);
                }
                {
                    auto rr = this->eval_expr(*b.right, env);
                    if (is_err(rr)) return make_err(rr.error());
                    RuntimeValue r = rr.value();
                    RuntimeValue out;
                    out.value = RuntimeValue::Bool{is_truthy(r)};
                    return ok(out);
                }
            }
            case Operator::Or: {
                if (is_truthy(l)) {
                    RuntimeValue out;
                    out.value = RuntimeValue::Bool{true};
                    return ok(out);
                }
                {
                    auto rr = this->eval_expr(*b.right, env);
                    if (is_err(rr)) return make_err(rr.error());
                    RuntimeValue r = rr.value();
                    RuntimeValue out;
                    out.value = RuntimeValue::Bool{is_truthy(r)};
                    return ok(out);
                }
            }
            default:
                break;
        }

        // Non-short-circuiting/other operators: evaluate RHS now
        auto rr = this->eval_expr(*b.right, env);
        if (is_err(rr)) return make_err(rr.error());
        RuntimeValue r = rr.value();

        switch (b.op) {
            case Operator::Add: {
                auto res = runtime_utils::numeric_add(l, r);
                if (is_ok(res)) return res;
                return err<RuntimeValue>(
                    std::make_shared<Error>("unsupported operand types for +", ErrorKind::Type));
            }
            case Operator::Sub: {
                auto res = runtime_utils::numeric_sub(l, r);
                if (is_ok(res)) return res;
                return err<RuntimeValue>(
                    std::make_shared<Error>("unsupported operand types for -", ErrorKind::Type));
            }
            case Operator::Mul: {
                auto res = runtime_utils::numeric_mul(l, r);
                if (is_ok(res)) return res;
                return err<RuntimeValue>(
                    std::make_shared<Error>("unsupported operand types for *", ErrorKind::Type));
            }
            case Operator::Div: {
                auto res = runtime_utils::numeric_div(l, r);
                if (is_ok(res)) return res;
                return err<RuntimeValue>(
                    std::make_shared<Error>("unsupported operand types for /", ErrorKind::Type));
            }
            case Operator::Pow: {
                auto res = runtime_utils::numeric_pow(l, r);
                if (is_ok(res)) return res;
                return err<RuntimeValue>(
                    std::make_shared<Error>("unsupported operand types for **", ErrorKind::Type));
            }
            case Operator::Eq: {
                bool eq = (l == r);
                RuntimeValue out;
                out.value = RuntimeValue::Bool{eq};
                return ok(out);
            }
            case Operator::Ne: {
                bool eq = (l == r);
                RuntimeValue out;
                out.value = RuntimeValue::Bool{!eq};
                return ok(out);
            }
            case Operator::Gt: {
                auto res = runtime_utils::compare_gt(l, r);
                if (is_ok(res)) return res;
                return err<RuntimeValue>(
                    std::make_shared<Error>("unsupported operand types for >", ErrorKind::Type));
            }
            case Operator::Lt: {
                auto res = runtime_utils::compare_lt(l, r);
                if (is_ok(res)) return res;
                return err<RuntimeValue>(
                    std::make_shared<Error>("unsupported operand types for <", ErrorKind::Type));
            }
            case Operator::Ge: {
                auto res = runtime_utils::compare_ge(l, r);
                if (is_ok(res)) return res;
                return err<RuntimeValue>(
                    std::make_shared<Error>("unsupported operand types for >=", ErrorKind::Type));
            }
            case Operator::Le: {
                auto res = runtime_utils::compare_le(l, r);
                if (is_ok(res)) return res;
                return err<RuntimeValue>(
                    std::make_shared<Error>("unsupported operand types for <=", ErrorKind::Type));
            }
            case Operator::Concat: {
                auto res = runtime_utils::concat(l, r);
                if (is_ok(res)) return res;
                return err<RuntimeValue>(
                    std::make_shared<Error>("unsupported operand types for ++", ErrorKind::Type));
            }
            default:
                return ok(RuntimeValue{RuntimeValue::Null{}});
        }
    }

    if (std::holds_alternative<E::FunctionCall>(expr.value)) {
        auto fc = std::get<E::FunctionCall>(expr.value);
        std::vector<RuntimeValue> eval_args;
        for (auto& a : fc.args) {
            auto ar = this->eval_expr(*a, env);
            if (is_err(ar)) return make_err(ar.error());
            eval_args.push_back(ar.value());
        }

        if (fc.name == "exit") std::exit(0);

        if (fc.name == "fstring") {
            if (eval_args.empty() ||
                !std::holds_alternative<RuntimeValue::String>(eval_args[0].value)) {
                return err<RuntimeValue>(std::make_shared<Error>(
                    "first argument to fstring must be a string template", ErrorKind::Type));
            }
            std::string tpl = std::get<RuntimeValue::String>(eval_args[0].value).value;
            std::string out;
            for (size_t i = 0; i < tpl.size(); ++i) {
                char c = tpl[i];
                if (c == '%' && i + 1 < tpl.size() &&
                    std::isdigit(static_cast<unsigned char>(tpl[i + 1]))) {
                    // parse number after '%'
                    size_t j = i + 1;
                    int num = 0;
                    while (j < tpl.size() && std::isdigit(static_cast<unsigned char>(tpl[j]))) {
                        num = num * 10 + (tpl[j] - '0');
                        ++j;
                    }
                    // placeholder indices are 1-based and refer to following args
                    if (num >= 1 && static_cast<size_t>(num) < eval_args.size()) {
                        out += to_string(eval_args[static_cast<size_t>(num)]);
                    }
                    // advance i to end of number
                    i = j - 1;
                } else {
                    out.push_back(c);
                }
            }
            RuntimeValue rv;
            rv.value = RuntimeValue::String{out};
            return ok(rv);
        }

        auto it = this->functions.find(fc.name);
        if (it != this->functions.end()) {
            MethodRepr& m = it->second;
            if (m.args.size() != eval_args.size())
                return err<RuntimeValue>(
                    std::make_shared<Error>("argument count mismatch", ErrorKind::Arity));
            auto child = std::make_shared<Environment>(env);
            // bind arguments by iterating map order
            std::size_t idx = 0;
            for (auto& kv : m.args) {
                const std::string& pname = kv.first;
                const AstType& pty = kv.second;
                if (!matches_type(eval_args[idx], pty))
                    return err<RuntimeValue>(
                        std::make_shared<Error>("argument type mismatch", ErrorKind::Type));
                child->set(pname, eval_args[idx]);
                ++idx;
            }
            auto flow = this->eval_statements(m.body, *child);
            if (is_err(flow)) return make_err(flow.error());
            if (std::holds_alternative<ExecFlow::Return>(flow.value().value)) {
                auto ret = std::get<ExecFlow::Return>(flow.value().value).value;
                if (!std::holds_alternative<AstType::Null>(m.returnType.value)) {
                    if (!matches_type(ret, m.returnType)) {
                        return err<RuntimeValue>(std::make_shared<Error>(
                            "function returned value that does not match declared return type",
                            ErrorKind::Type));
                    }
                }
                return ok(ret);
            }
            if (std::holds_alternative<ExecFlow::None>(flow.value().value)) {
                if (!std::holds_alternative<AstType::Null>(m.returnType.value)) {
                    return err<RuntimeValue>(std::make_shared<Error>(
                        "function did not return a value but has declared return type",
                        ErrorKind::Type));
                }
                return ok(RuntimeValue{RuntimeValue::Null{}});
            }
            if (std::holds_alternative<ExecFlow::Break>(flow.value().value) ||
                std::holds_alternative<ExecFlow::Continue>(flow.value().value)) {
                return err<RuntimeValue>(std::make_shared<Error>(
                    "unexpected control flow in function body", ErrorKind::Runtime));
            }
        }

        return ok(RuntimeValue{RuntimeValue::Null{}});
    }

    if (std::holds_alternative<E::Ternary>(expr.value)) {
        auto& t = std::get<E::Ternary>(expr.value);
        auto cr = this->eval_expr(*t.condition, env);
        if (is_err(cr)) return make_err(cr.error());
        if (is_truthy(cr.value()))
            return this->eval_expr(*t.then_expr, env);
        else
            return this->eval_expr(*t.else_expr, env);
    }

    return ok(RuntimeValue{RuntimeValue::Null{}});
}