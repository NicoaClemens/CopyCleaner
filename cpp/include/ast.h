// ast.h
// Declares: AstType, Operator, Expr, Statement

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "ast_common.hpp"
#include "runtime_value.h"

struct AstType {
    AstType() = default;
    AstType(const AstType& other);
    AstType& operator=(const AstType& other);

    AstType(AstType&&) noexcept = default;
    AstType& operator=(AstType&&) noexcept = default;

    struct Int {};
    struct Float {};
    struct Bool {};
    struct String {};
    struct Regex {};
    struct Match {};
    struct Null {};
    struct List {
        std::unique_ptr<AstType> element;
    };

    using Variant = std::variant<Int, Float, Bool, String, Regex, Match, Null, List>;

    Variant value;
};

/// @brief creates an AstType object with `value` set to AstType::Null
/// @return AstType with value set to AstType::Null
inline AstType astCreateNull() {
    auto a = AstType();
    a.value = AstType::Null{};
    return a;
}

enum class Operator {
    /// Addition (+)
    Add,
    /// Subtraction (-)
    Sub,
    /// Multiplication (*)
    Mul,
    /// Division (/)
    Div,
    /// Exponentiation (**)
    Pow,
    /// Equality (==)
    Eq,
    /// Not equality (!=)
    Ne,
    /// Greater Than (>)
    Gt,
    /// Less Than (<)
    Lt,
    /// Greater or equal (>=)
    Ge,
    /// Less or equal (<=)
    Le,
    /// Logical AND (&&)
    And,
    /// Logical OR (||)
    Or,
    /// Logical Not (!)
    Not,
    /// Numerical negation (-) unary
    Neg,
    /// String concat (++)
    Concat,
};

struct Expr;
using ExprPtr = std::unique_ptr<Expr>;
struct RuntimeValue;
struct Expr {
    Expr() = default;
    Expr(const Expr& other);
    Expr& operator=(const Expr& other);

    Expr(Expr&&) noexcept = default;
    Expr& operator=(Expr&&) noexcept = default;

    struct Literal {
        RuntimeValue value;
    };
    struct Variable {
        std::string name;
    };
    struct UnaryOp {
        Operator op;
        ExprPtr next;
    };
    struct BinaryOp {
        ExprPtr left;
        Operator op;
        ExprPtr right;
    };
    struct FunctionCall {
        std::string name;
        std::vector<ExprPtr> args;
    };
    struct Ternary {
        ExprPtr condition;
        ExprPtr then_expr;
        ExprPtr else_expr;
    };
    struct ListLiteral {
        std::vector<ExprPtr> elements;
    };
    struct TypeCast {
        AstType target_type;
        ExprPtr expr;
    };
    struct MemberAccess {
        ExprPtr object;
        std::string member;
    };

    using Variant = std::variant<Literal, Variable, UnaryOp, BinaryOp, FunctionCall, Ternary,
                                 ListLiteral, TypeCast, MemberAccess>;

    Span span;
    Variant value;
};

struct Statement;
using StmtPtr = std::unique_ptr<Statement>;
struct Statement {
    Statement() = default;
    Statement(const Statement& other);
    Statement& operator=(const Statement& other);

    Statement(Statement&&) noexcept = default;
    Statement& operator=(Statement&&) noexcept = default;

    struct Assignment {
        std::string name;
        Expr expr;
    };

    struct VarDecl {
        std::string name;
        AstType type;
        std::optional<Expr> initializer;
    };

    struct If {
        Expr condition;
        std::vector<StmtPtr> body;
        std::vector<std::pair<Expr, std::vector<StmtPtr>>> elif;
        std::vector<StmtPtr> else_body;
    };

    struct While {
        Expr condition;
        std::vector<StmtPtr> body;
    };

    struct Return {
        Expr value;
    };

    struct FunctionDef {
        std::string name;
        std::vector<std::pair<std::string, AstType>> params;
        std::vector<StmtPtr> body;
        std::optional<AstType> return_type;
    };

    struct Break {};
    struct Continue {};

    struct ExpressionStmt {
        Expr expr;
    };

    using Variant = std::variant<Assignment, VarDecl, If, While, Return, FunctionDef, Break,
                                 Continue, ExpressionStmt>;

    Variant value;
};