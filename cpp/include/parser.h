// parser.h
// Declares: Parser

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ast.h"
#include "lexer.h"
#include "result.hpp"

namespace parser {

class Parser {
   public:
    explicit Parser(lexer::Lexer& lexer);

    Result<std::vector<Statement>> parse();

   private:
    // Statement parsing
    Result<Statement> parse_statement();
    Result<Statement> parse_assignment();
    Result<Statement> parse_if_statement();
    Result<Statement> parse_while_statement();
    Result<Statement> parse_function_def();
    Result<Statement> parse_return_statement();

    // Expression parsing (precedence climbing)
    Result<Expr> parse_expression();
    Result<Expr> parse_ternary();
    Result<Expr> parse_logical_or();
    Result<Expr> parse_logical_and();
    Result<Expr> parse_comparison();
    Result<Expr> parse_addition();
    Result<Expr> parse_multiplication();
    Result<Expr> parse_exponentiation();
    Result<Expr> parse_unary();
    Result<Expr> parse_primary();

    // Type parsing
    Result<AstType> parse_type();

    // Helpers
    lexer::Token peek();
    lexer::Token advance();
    bool check(lexer::TokenKind kind);
    bool match(lexer::TokenKind kind);
    Result<lexer::Token> expect(lexer::TokenKind kind, const std::string& msg);

    lexer::Lexer& lexer_;
    lexer::Token current_;
    bool had_error_ = false;
};

}  // namespace parser
