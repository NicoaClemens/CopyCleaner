// ast.cpp
// Implements ast.h

#include "ast.h"

#include "utils.hpp"

AstType::AstType(const AstType& other) {
    value = std::visit([](const auto& v) -> Variant {
        using T = std::decay_t<decltype(v)>;

        if constexpr (std::is_same_v<T, List>) {
            return List { utils::clone(v.element) };
        }
        else {
            return v;
        }
    }, other.value);
}

AstType& AstType::operator=(const AstType& other) {
    if (this != &other) {
        AstType tmp(other);
        value = std::move(tmp.value);
    }
    return *this;
}

Expr::Expr(const Expr& other) {
    value = std::visit([](const auto& v) -> Variant {
        using T = std::decay_t<decltype(v)>;

        if constexpr (std::is_same_v<T, UnaryOp>) {
            return UnaryOp { v.op, utils::clone(v.next) };
        }
        else if constexpr (std::is_same_v<T, BinaryOp>) {
            return BinaryOp { utils::clone(v.left), v.op, utils::clone(v.right) };
        }
        else if constexpr (std::is_same_v<T, FunctionCall>) {
            return FunctionCall { v.name, utils::clone(v.args) };
        }
        else if constexpr (std::is_same_v<T, Ternary>) {
            return Ternary { utils::clone(v.condition), utils::clone(v.then_expr), utils::clone(v.else_expr) };
        }
        else {
            return v;
        }
    }, other.value);
}

Expr& Expr::operator=(const Expr& other) {
    if (this != &other) {
        Expr tmp(other);
        value = std::move(tmp.value);
    }
    return *this;
}

Statement::Statement(const Statement& other) {
    value = std::visit([](const auto& v) -> Variant {
        using T = std::decay_t<decltype(v)>;

        if constexpr (std::is_same_v<T, If>) {
            return If{ 
                v.condition, 
                utils::clone(v.body), 
                [&] {
                    std::vector<std::pair<Expr, std::vector<StmtPtr>>> r;
                    for (const auto& [cond, body] : v.elif) r.emplace_back(cond, utils::clone(body));
                    return r;
                }(),
                utils::clone(v.else_body)
            };
        }
        else if constexpr (std::is_same_v<T, While>) {
            return While{ v.condition, utils::clone(v.body) };
        }
        else if constexpr (std::is_same_v<T, FunctionDef>) {
            return FunctionDef{
                v.name,
                v.params,
                utils::clone(v.body),
                v.return_type
            };
        }
        else {
            return v;
        }
    }, other.value);    
}

Statement& Statement::operator=(const Statement& other) {
    if (this != &other) {
        Statement tmp(other);
        value = std::move(tmp.value);
    }
    return *this;
}
