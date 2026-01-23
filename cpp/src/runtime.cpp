// runtime.cpp

#include <cmath>
#include <cstdlib>

#include "runtime.h"
#include "types.hpp"
#include "runtime_utils.h"

std::optional<RuntimeValue> Environment::get(const std::string& name) {
    if (variables.contains(name)) return variables[name];
    if (parent) return parent->get(name);
    return std::nullopt;
}

void Environment::set(const std::string& name, const RuntimeValue& value) {
    variables[name] = value;
}

Result<RuntimeValue> Interpreter::run(std::vector<Statement>& stmts) {
    Result<ExecFlow> r = this->eval_statements(stmts, *this->global_env);
    if (r.success == ResultType::Error) return Result<RuntimeValue>{ RuntimeValue::Null{}, r.success, r.error.value() };
    std::visit(overloaded{
        [](ExecFlow::None) {
            return Result{ RuntimeValue::Null{}, ResultType::OK };
        },
        [](ExecFlow::Return& r) {
            return Result<RuntimeValue>{ RuntimeValue{ r.value }, ResultType::OK };
        },
        [](ExecFlow::Break) {
            return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::Error, std::make_shared<SyntaxError>("invalid 'break' statement", nullptr) };
        },
        [](ExecFlow::Continue) {
            return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::Error, std::make_shared<SyntaxError>("invalid 'continue' statement", nullptr) };
        }
    }, r.value.value);
}

Result<ExecFlow> Interpreter::eval_statements(std::vector<Statement>& stmts, Environment& env) {
    for (auto &s : stmts) {
        
        // Assignment
        if (auto a = std::get_if<Statement::Assignment>(&s.value)) {
            auto r = this->eval_expr(a->expr, env.shared_from_this());
            if (r.success == ResultType::Error) return Result<ExecFlow>{ ExecFlow{}, ResultType::Error, r.error };
            env.set(a->name, r.value);
            continue;
        }

        // If
        if (auto i = std::get_if<Statement::If>(&s.value)) {
            auto cond_r = this->eval_expr(i->condition, env.shared_from_this());
            if (cond_r.success == ResultType::Error) return Result<ExecFlow>{ ExecFlow{}, ResultType::Error, cond_r.error };
            if (is_truthy(cond_r.value)) {
                auto child = std::make_shared<Environment>(env.shared_from_this());
                std::vector<Statement> body_vals;
                for (auto &sp : i->body) body_vals.push_back(*sp);
                auto flow = this->eval_statements(body_vals, *child);
                if (flow.success == ResultType::Error) return Result<ExecFlow>{ ExecFlow{}, ResultType::Error, flow.error };
                if (!std::holds_alternative<ExecFlow::None>(flow.value.value)) return flow;
            } else {
                bool matched = false;
                for (auto &el : i->elif) {
                    auto er = this->eval_expr(el.first, env.shared_from_this());
                    if (er.success == ResultType::Error) return Result<ExecFlow>{ ExecFlow{}, ResultType::Error, er.error };
                    if (is_truthy(er.value)) {
                        matched = true;
                        auto child = std::make_shared<Environment>(env.shared_from_this());
                        std::vector<Statement> body_vals;
                        for (auto &sp : el.second) body_vals.push_back(*sp);
                        auto flow = this->eval_statements(body_vals, *child);
                        if (flow.success == ResultType::Error) return Result<ExecFlow>{ ExecFlow{}, ResultType::Error, flow.error };
                        if (!std::holds_alternative<ExecFlow::None>(flow.value.value)) return flow;
                        break;
                    }
                }
                if (!matched) {
                    auto child = std::make_shared<Environment>(env.shared_from_this());
                    std::vector<Statement> body_vals;
                    for (auto &sp : i->else_body) body_vals.push_back(*sp);
                    auto flow = this->eval_statements(body_vals, *child);
                    if (flow.success == ResultType::Error) return Result<ExecFlow>{ ExecFlow{}, ResultType::Error, flow.error };
                    if (!std::holds_alternative<ExecFlow::None>(flow.value.value)) return flow;
                }
            }
            continue;
        }

        // While
        if (auto w = std::get_if<Statement::While>(&s.value)) {
            while (true) {
                auto cr = this->eval_expr(w->condition, env.shared_from_this());
                if (cr.success == ResultType::Error) return Result<ExecFlow>{ ExecFlow{}, ResultType::Error, cr.error };
                if (!is_truthy(cr.value)) break;
                auto child = std::make_shared<Environment>(env.shared_from_this());
                std::vector<Statement> body_vals;
                for (auto &sp : w->body) body_vals.push_back(*sp);
                auto flow = this->eval_statements(body_vals, *child);
                if (flow.success == ResultType::Error) return Result<ExecFlow>{ ExecFlow{}, ResultType::Error, flow.error };
                if (std::holds_alternative<ExecFlow::Return>(flow.value.value)) return flow;
                if (std::holds_alternative<ExecFlow::Break>(flow.value.value)) break;
                if (std::holds_alternative<ExecFlow::Continue>(flow.value.value)) continue;
            }
            continue;
        }

        // Return
        if (auto r = std::get_if<Statement::Return>(&s.value)) {
            auto vr = this->eval_expr(r->value, env.shared_from_this());
            if (vr.success == ResultType::Error) return Result<ExecFlow>{ ExecFlow{}, ResultType::Error, vr.error };
            ExecFlow out; out.value = ExecFlow::Return{ vr.value };
            return Result<ExecFlow>{ out, ResultType::OK };
        }

        // FunctionDef
        if (auto f = std::get_if<Statement::FunctionDef>(&s.value)) {
            MethodRepr m;
            for (auto &p : f->params) m.args.emplace(p.first, p.second);
            m.returnType = f->return_type.value_or(AstType{ AstType::Null{} });
            for (auto &sp : f->body) m.body.push_back(*sp);
            this->functions[f->name] = std::move(m);
            continue;   
        }

        // Break
        if (std::holds_alternative<Statement::Break>(s.value)) {
            ExecFlow out; out.value = ExecFlow::Break{}; return Result<ExecFlow>{ out, ResultType::OK };
        }

        // Continue
        if (std::holds_alternative<Statement::Continue>(s.value)) {
            ExecFlow out; out.value = ExecFlow::Continue{}; return Result<ExecFlow>{ out, ResultType::OK };
        }
    }

    ExecFlow none; none.value = ExecFlow::None{};
    return Result<ExecFlow>{ none, ResultType::OK };
}


Result<RuntimeValue> Interpreter::eval_expr(Expr& expr, envPtr env) {
    using E = Expr;

    auto make_err = [&](std::shared_ptr<Error> e)-> Result<RuntimeValue> { return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::Error, e }; };

    if (std::holds_alternative<E::Literal>(expr.value)) {
        auto lit = std::get<E::Literal>(expr.value);
        return Result<RuntimeValue>{ lit.value, ResultType::OK };
    }

    if (std::holds_alternative<E::Variable>(expr.value)) {
        auto v = std::get<E::Variable>(expr.value);
        if (!env) return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
        auto ov = env->get(v.name);
        if (ov.has_value()) return Result<RuntimeValue>{ ov.value(), ResultType::OK };
        return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
    }

    if (std::holds_alternative<E::UnaryOp>(expr.value)) {
        const auto &u = std::get<E::UnaryOp>(expr.value);
        auto r = this->eval_expr(*u.next, env);
        if (r.success == ResultType::Error) return make_err(r.error.value());
        if (u.op == Operator::Not) {
            if (std::holds_alternative<RuntimeValue::Bool>(r.value.value)) {
                bool b = std::get<RuntimeValue::Bool>(r.value.value).value;
                RuntimeValue out; out.value = RuntimeValue::Bool{ !b };
                return Result<RuntimeValue>{ out, ResultType::OK };
            }
            return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
        }
        return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
    }

    if (std::holds_alternative<E::BinaryOp>(expr.value)) {
        const auto &b = std::get<E::BinaryOp>(expr.value);
        auto lr = this->eval_expr(*b.left, env);
        if (lr.success == ResultType::Error) return make_err(lr.error.value());
        auto rr = this->eval_expr(*b.right, env);
        if (rr.success == ResultType::Error) return make_err(rr.error.value());
        RuntimeValue l = lr.value;
        RuntimeValue r = rr.value;

        // helpers in runtime_utils.hpp

        switch (b.op) {
            case Operator::Add: {
                if (auto res = runtime_utils::numeric_add(l, r); res.has_value()) return Result<RuntimeValue>{ res.value(), ResultType::OK };
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            }
            case Operator::Sub: {
                if (auto res = runtime_utils::numeric_sub(l, r); res.has_value()) return Result<RuntimeValue>{ res.value(), ResultType::OK };
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            }
            case Operator::Mul: {
                if (auto res = runtime_utils::numeric_mul(l, r); res.has_value()) return Result<RuntimeValue>{ res.value(), ResultType::OK };
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            }
            case Operator::Div: {
                if (auto res = runtime_utils::numeric_div(l, r); res.has_value()) return Result<RuntimeValue>{ res.value(), ResultType::OK };
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            }
            case Operator::Pow: {
                if (auto res = runtime_utils::numeric_pow(l, r); res.has_value()) return Result<RuntimeValue>{ res.value(), ResultType::OK };
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            }
            case Operator::Eq: {
                bool eq = (l == r);
                RuntimeValue out; out.value = RuntimeValue::Bool{ eq };
                return Result<RuntimeValue>{ out, ResultType::OK };
            }
            case Operator::Ne: {
                bool eq = (l == r);
                RuntimeValue out; out.value = RuntimeValue::Bool{ !eq };
                return Result<RuntimeValue>{ out, ResultType::OK };
            }
            case Operator::Gt: {
                if (auto res = runtime_utils::compare_gt(l, r); res.has_value()) return Result<RuntimeValue>{ res.value(), ResultType::OK };
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            }
            case Operator::Lt: {
                if (auto res = runtime_utils::compare_lt(l, r); res.has_value()) return Result<RuntimeValue>{ res.value(), ResultType::OK };
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            }
            case Operator::Ge: {
                if (auto res = runtime_utils::compare_ge(l, r); res.has_value()) return Result<RuntimeValue>{ res.value(), ResultType::OK };
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            }
            case Operator::Le: {
                if (auto res = runtime_utils::compare_le(l, r); res.has_value()) return Result<RuntimeValue>{ res.value(), ResultType::OK };
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            }
            case Operator::And: {
                if (auto res = runtime_utils::bool_and(l, r); res.has_value()) return Result<RuntimeValue>{ res.value(), ResultType::OK };
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            }
            case Operator::Or: {
                if (auto res = runtime_utils::bool_or(l, r); res.has_value()) return Result<RuntimeValue>{ res.value(), ResultType::OK };
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            }
            case Operator::Concat: {
                if (auto res = runtime_utils::concat(l, r); res.has_value()) return Result<RuntimeValue>{ res.value(), ResultType::OK };
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            }
            default:
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
        }
    }

    if (std::holds_alternative<E::FunctionCall>(expr.value)) {
        auto fc = std::get<E::FunctionCall>(expr.value);
        std::vector<RuntimeValue> eval_args;
        for (auto &a : fc.args) {
            auto ar = this->eval_expr(*a, env);
            if (ar.success == ResultType::Error) return make_err(ar.error.value());
            eval_args.push_back(ar.value);
        }

        if (fc.name == "exit") std::exit(0);

        auto it = this->functions.find(fc.name);
        if (it != this->functions.end()) {
            MethodRepr &m = it->second;
            if (m.args.size() != eval_args.size()) return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            auto child = std::make_shared<Environment>(env);
            // bind arguments by iterating map order
            std::size_t idx = 0;
            for (auto &kv : m.args) {
                const std::string &pname = kv.first;
                const AstType &pty = kv.second;
                if (!matches_type(eval_args[idx], pty)) return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
                child->set(pname, eval_args[idx]);
                ++idx;
            }
            auto flow = this->eval_statements(m.body, *child);
            if (flow.success == ResultType::Error) return make_err(flow.error.value());
            if (std::holds_alternative<ExecFlow::Return>(flow.value.value)) {
                auto ret = std::get<ExecFlow::Return>(flow.value.value).value;
                if (!std::holds_alternative<AstType::Null>(m.returnType.value)) {
                    if (!matches_type(ret, m.returnType)) {
                        auto e = std::make_shared<RuntimeError>();
                        e->message = "function '" + fc.name + "' returned value that does not match declared return type";
                        e->span = nullptr;
                        return make_err(e);
                    }
                }
                return Result<RuntimeValue>{ ret, ResultType::OK };
            }
            if (std::holds_alternative<ExecFlow::None>(flow.value.value)) {
                if (!std::holds_alternative<AstType::Null>(m.returnType.value)) {
                    auto e = std::make_shared<RuntimeError>();
                    e->message = "function '" + fc.name + "' did not return a value but has declared return type";
                    e->span = nullptr;
                    return make_err(e);
                }
                return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
            }
            if (std::holds_alternative<ExecFlow::Break>(flow.value.value) || std::holds_alternative<ExecFlow::Continue>(flow.value.value)) {
                auto e = std::make_shared<RuntimeError>();
                e->message = "unexpected control flow in function body";
                e->span = nullptr;
                return make_err(e);
            }
        }

        return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
    }

    if (std::holds_alternative<E::Ternary>(expr.value)) {
        auto &t = std::get<E::Ternary>(expr.value);
        auto cr = this->eval_expr(*t.condition, env);
        if (cr.success == ResultType::Error) return make_err(cr.error.value());
        if (is_truthy(cr.value)) return this->eval_expr(*t.then_expr, env);
        else return this->eval_expr(*t.else_expr, env);
    }

    return Result<RuntimeValue>{ RuntimeValue::Null{}, ResultType::OK };
}