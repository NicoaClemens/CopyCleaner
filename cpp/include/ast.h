// ast.h

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "runtime_value.h"

struct Pos {
    std::size_t line;
    std::size_t column;

    bool operator==(const Pos& other) const { return (line == other.line) && (column == other.column); }
    bool operator!=(const Pos& other) const { return !operator==(other); }
};

struct Span {
    Pos p1;
    Pos p2;
};

template <typename T>
struct Node {
    T value;
    Span s;
};

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
    struct List { std::unique_ptr<AstType> element; };

    using Variant = std::variant<
        Int, Float, Bool, String, Regex, Match, Null, List>;

    Variant value;
};

AstType astCreateNull() { auto a = AstType(); a.value = AstType::Null{}; return a; }

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
struct Expr{

    Expr() = default;
    Expr(const Expr& other);
    Expr& operator=(const Expr& other);

    Expr(Expr&&) noexcept = default;
    Expr& operator=(Expr&&) noexcept = default;

    struct Literal { RuntimeValue value; };
    struct Variable { std::string name; };
    struct UnaryOp { Operator op; ExprPtr next; };
    struct BinaryOp { ExprPtr left; Operator op; ExprPtr right; };
    struct FunctionCall { std::string name; std::vector<ExprPtr> args; };
    struct Ternary { ExprPtr condition; ExprPtr then_expr; ExprPtr else_expr; };

    using Variant = std::variant<
        Literal, Variable, UnaryOp, BinaryOp, FunctionCall, Ternary>;

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

    struct Assignment { std::string name; Expr expr; };

    struct If {
        Expr condition;
        std::vector<StmtPtr> body;
        std::vector<std::pair<Expr, std::vector<StmtPtr>>> elif;
        std::vector<StmtPtr> else_body;
    };

    struct While { Expr condition; std::vector<StmtPtr> body; };

    struct Return { Expr value; };

    struct FunctionDef {
        std::string name;
        std::vector<std::pair<std::string, AstType>> params;
        std::vector<StmtPtr> body;
        std::optional<AstType> return_type;
    };

    struct Break {};
    struct Continue {};

    using Variant = std::variant<
        Assignment, If, While, Return, FunctionDef, Break, Continue>;

    Variant value;
};