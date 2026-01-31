// parser.cpp
// Implements parser.h

#include "parser.h"

#include <memory>
#include <vector>

#include "ast.h"
#include "errors.hpp"
#include "lexer.h"
#include "result.hpp"

using lexer::Lexer;
using lexer::Token;
using lexer::TokenKind;

namespace parser {

Parser::Parser(Lexer& lexer) : lexer_(lexer) {
    auto token_result = lexer_.next_token();
    if (token_result.is_err()) {
        had_error_ = true;
        // Use a dummy token as placeholder; error will be reported in parse()
        current_ = Token{TokenKind::Unknown, "", Span{}};
    } else {
        current_ = token_result.value();
    }
}

Result<std::vector<Statement>> Parser::parse() {
    if (had_error_) {
        return err<std::vector<Statement>>(
            std::make_shared<Error>("Failed to initialize parser: lexer error", ErrorKind::Parse));
    }
    std::vector<Statement> statements;

    while (!check(TokenKind::EndOfFile)) {
        auto stmt = parse_statement();
        if (is_err(stmt)) {
            return err<std::vector<Statement>>(stmt.error());
        }
        statements.push_back(std::move(stmt).value());
    }

    return ok(std::move(statements));
}

// Helper methods
Token Parser::peek() {
    return current_;
}

Token Parser::advance() {
    Token prev = current_;
    auto result = lexer_.next_token();
    if (is_ok(result)) {
        current_ = result.value();
    }
    return prev;
}

bool Parser::check(TokenKind kind) {
    return current_.kind == kind;
}

bool Parser::match(TokenKind kind) {
    if (check(kind)) {
        advance();
        return true;
    }
    return false;
}

Result<Token> Parser::expect(TokenKind kind, const std::string& msg) {
    if (check(kind)) {
        return ok(advance());
    }
    return err<Token>(std::make_shared<Error>(msg, current_.span, ErrorKind::Syntax));
}

// Statement parsing
Result<Statement> Parser::parse_statement() {
    // function keyword
    if (match(TokenKind::KwFunction)) {
        return parse_function_def();
    }

    // Control flow
    if (match(TokenKind::KwIf)) {
        return parse_if_statement();
    }
    if (match(TokenKind::KwWhile)) {
        return parse_while_statement();
    }
    if (match(TokenKind::KwReturn)) {
        return parse_return_statement();
    }
    if (match(TokenKind::KwBreak)) {
        auto tok = expect(TokenKind::Semicolon, "expected ';' after 'break'");
        if (is_err(tok)) return err<Statement>(tok.error());
        Statement stmt;
        stmt.value = Statement::Break{};
        return ok(stmt);
    }
    if (match(TokenKind::KwContinue)) {
        auto tok = expect(TokenKind::Semicolon, "expected ';' after 'continue'");
        if (is_err(tok)) return err<Statement>(tok.error());
        Statement stmt;
        stmt.value = Statement::Continue{};
        return ok(stmt);
    }

    // Variable declaration or assignment
    if (check(TokenKind::Identifier)) {
        // Check if this is a type keyword (potential variable declaration)
        std::string ident = peek().lexeme;
        if (ident == "int" || ident == "float" || ident == "boolean" || 
            ident == "string" || ident == "regex" || ident == "match" || ident == "list") {
            return parse_var_declaration();
        }
        // Otherwise it's an assignment
        return parse_assignment();
    }

    return err<Statement>(
        std::make_shared<Error>("unexpected token in statement", current_.span, ErrorKind::Syntax));
}

Result<Statement> Parser::parse_assignment() {
    auto name_tok = expect(TokenKind::Identifier, "expected identifier");
    if (is_err(name_tok)) return err<Statement>(name_tok.error());

    auto eq_tok = expect(TokenKind::Assign, "expected '='");
    if (is_err(eq_tok)) return err<Statement>(eq_tok.error());

    auto expr = parse_expression();
    if (is_err(expr)) return err<Statement>(expr.error());

    auto semi = expect(TokenKind::Semicolon, "expected ';' after assignment");
    if (is_err(semi)) return err<Statement>(semi.error());

    Statement stmt;
    stmt.value = Statement::Assignment{name_tok.value().lexeme, expr.value()};
    return ok(stmt);
}

Result<Statement> Parser::parse_var_declaration() {
    // Parse type
    auto type_result = parse_type();
    if (is_err(type_result)) return err<Statement>(type_result.error());
    AstType type = std::move(type_result).value();

    // Parse variable name
    auto name_tok = expect(TokenKind::Identifier, "expected variable name after type");
    if (is_err(name_tok)) return err<Statement>(name_tok.error());
    std::string name = name_tok.value().lexeme;

    // Parse initializer: (expr) or () or () = expr
    auto lparen = expect(TokenKind::LParen, "expected '(' after variable name");
    if (is_err(lparen)) return err<Statement>(lparen.error());

    std::optional<Expr> initializer;
    
    // Check for empty initializer: int n()
    if (!check(TokenKind::RParen)) {
        // Has expression inside parens: int n(4)
        auto expr = parse_expression();
        if (is_err(expr)) return err<Statement>(expr.error());
        initializer = std::move(expr).value();
    }

    auto rparen = expect(TokenKind::RParen, "expected ')' after initializer");
    if (is_err(rparen)) return err<Statement>(rparen.error());

    // Check for assignment syntax: int n() = expr
    if (match(TokenKind::Assign)) {
        auto expr = parse_expression();
        if (is_err(expr)) return err<Statement>(expr.error());
        initializer = std::move(expr).value();
    }

    auto semi = expect(TokenKind::Semicolon, "expected ';' after variable declaration");
    if (is_err(semi)) return err<Statement>(semi.error());

    Statement stmt;
    stmt.value = Statement::VarDecl{name, type, initializer};
    return ok(stmt);
}

Result<Statement> Parser::parse_if_statement() {
    auto lparen = expect(TokenKind::LParen, "expected '(' after 'if'");
    if (is_err(lparen)) return err<Statement>(lparen.error());

    auto cond = parse_expression();
    if (is_err(cond)) return err<Statement>(cond.error());

    auto rparen = expect(TokenKind::RParen, "expected ')' after if condition");
    if (is_err(rparen)) return err<Statement>(rparen.error());

    auto lbrace = expect(TokenKind::LBrace, "expected '{' after if condition");
    if (is_err(lbrace)) return err<Statement>(lbrace.error());

    std::vector<StmtPtr> body;
    while (!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile)) {
        auto stmt = parse_statement();
        if (is_err(stmt)) return err<Statement>(stmt.error());
        body.push_back(std::make_unique<Statement>(std::move(stmt).value()));
    }

    auto rbrace = expect(TokenKind::RBrace, "expected '}' after if body");
    if (is_err(rbrace)) return err<Statement>(rbrace.error());

    // elif clauses
    std::vector<std::pair<Expr, std::vector<StmtPtr>>> elif_clauses;
    while (match(TokenKind::KwElif)) {
        auto elif_lparen = expect(TokenKind::LParen, "expected '(' after 'elif'");
        if (is_err(elif_lparen)) return err<Statement>(elif_lparen.error());

        auto elif_cond = parse_expression();
        if (is_err(elif_cond)) return err<Statement>(elif_cond.error());

        auto elif_rparen = expect(TokenKind::RParen, "expected ')' after elif condition");
        if (is_err(elif_rparen)) return err<Statement>(elif_rparen.error());

        auto elif_lbrace = expect(TokenKind::LBrace, "expected '{' after elif condition");
        if (is_err(elif_lbrace)) return err<Statement>(elif_lbrace.error());

        std::vector<StmtPtr> elif_body;
        while (!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile)) {
            auto stmt = parse_statement();
            if (is_err(stmt)) return err<Statement>(stmt.error());
            elif_body.push_back(std::make_unique<Statement>(std::move(stmt).value()));
        }

        auto elif_rbrace = expect(TokenKind::RBrace, "expected '}' after elif body");
        if (is_err(elif_rbrace)) return err<Statement>(elif_rbrace.error());

        elif_clauses.emplace_back(elif_cond.value(), std::move(elif_body));
    }

    // else clause
    std::vector<StmtPtr> else_body;
    if (match(TokenKind::KwElse)) {
        auto else_lbrace = expect(TokenKind::LBrace, "expected '{' after 'else'");
        if (is_err(else_lbrace)) return err<Statement>(else_lbrace.error());

        while (!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile)) {
            auto stmt = parse_statement();
            if (is_err(stmt)) return err<Statement>(stmt.error());
            else_body.push_back(std::make_unique<Statement>(std::move(stmt).value()));
        }

        auto else_rbrace = expect(TokenKind::RBrace, "expected '}' after else body");
        if (is_err(else_rbrace)) return err<Statement>(else_rbrace.error());
    }

    auto semi = expect(TokenKind::Semicolon, "expected ';' after if statement");
    if (is_err(semi)) return err<Statement>(semi.error());

    Statement stmt;
    stmt.value =
        Statement::If{cond.value(), std::move(body), std::move(elif_clauses), std::move(else_body)};
    return ok(stmt);
}

Result<Statement> Parser::parse_while_statement() {
    auto lparen = expect(TokenKind::LParen, "expected '(' after 'while'");
    if (is_err(lparen)) return err<Statement>(lparen.error());

    auto cond = parse_expression();
    if (is_err(cond)) return err<Statement>(cond.error());

    auto rparen = expect(TokenKind::RParen, "expected ')' after while condition");
    if (is_err(rparen)) return err<Statement>(rparen.error());

    auto lbrace = expect(TokenKind::LBrace, "expected '{' after while condition");
    if (is_err(lbrace)) return err<Statement>(lbrace.error());

    std::vector<StmtPtr> body;
    while (!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile)) {
        auto stmt = parse_statement();
        if (is_err(stmt)) return err<Statement>(stmt.error());
        body.push_back(std::make_unique<Statement>(std::move(stmt).value()));
    }

    auto rbrace = expect(TokenKind::RBrace, "expected '}' after while body");
    if (is_err(rbrace)) return err<Statement>(rbrace.error());

    auto semi = expect(TokenKind::Semicolon, "expected ';' after while statement");
    if (is_err(semi)) return err<Statement>(semi.error());

    Statement stmt;
    stmt.value = Statement::While{cond.value(), std::move(body)};
    return ok(stmt);
}

Result<Statement> Parser::parse_function_def() {
    auto name_tok = expect(TokenKind::Identifier, "expected function name");
    if (is_err(name_tok)) return err<Statement>(name_tok.error());
    std::string func_name = name_tok.value().lexeme;

    // Parse return type if present (BEFORE parameters as per docs: "function name returns type(params)")
    std::optional<AstType> return_type;
    if (match(TokenKind::KwReturns)) {
        auto ret_type = parse_type();
        if (is_err(ret_type)) return err<Statement>(ret_type.error());
        return_type = ret_type.value();
    }

    auto lparen = expect(TokenKind::LParen, "expected '(' after function name");
    if (is_err(lparen)) return err<Statement>(lparen.error());

    // Parse parameters
    std::vector<std::pair<std::string, AstType>> params;
    if (!check(TokenKind::RParen)) {
        do {
            auto param_type = parse_type();
            if (is_err(param_type)) return err<Statement>(param_type.error());

            auto param_name = expect(TokenKind::Identifier, "expected parameter name");
            if (is_err(param_name)) return err<Statement>(param_name.error());

            params.emplace_back(param_name.value().lexeme, std::move(param_type).value());
        } while (match(TokenKind::Comma));
    }

    auto rparen = expect(TokenKind::RParen, "expected ')' after parameters");
    if (is_err(rparen)) return err<Statement>(rparen.error());

    auto lbrace = expect(TokenKind::LBrace, "expected '{' after function signature");
    if (is_err(lbrace)) return err<Statement>(lbrace.error());

    std::vector<StmtPtr> body;
    while (!check(TokenKind::RBrace) && !check(TokenKind::EndOfFile)) {
        auto stmt = parse_statement();
        if (is_err(stmt)) return err<Statement>(stmt.error());
        body.push_back(std::make_unique<Statement>(std::move(stmt).value()));
    }

    auto rbrace = expect(TokenKind::RBrace, "expected '}' after function body");
    if (is_err(rbrace)) return err<Statement>(rbrace.error());

    auto semi = expect(TokenKind::Semicolon, "expected ';' after function definition");
    if (is_err(semi)) return err<Statement>(semi.error());

    Statement stmt;
    stmt.value = Statement::FunctionDef{func_name, params, std::move(body), return_type};
    return ok(stmt);
}

Result<Statement> Parser::parse_return_statement() {
    auto expr = parse_expression();
    if (is_err(expr)) return err<Statement>(expr.error());

    auto semi = expect(TokenKind::Semicolon, "expected ';' after return statement");
    if (is_err(semi)) return err<Statement>(semi.error());

    Statement stmt;
    stmt.value = Statement::Return{expr.value()};
    return ok(stmt);
}

// Type parsing
Result<AstType> Parser::parse_type() {
    auto type_tok = expect(TokenKind::Identifier, "expected type name");
    if (is_err(type_tok)) return err<AstType>(type_tok.error());

    std::string type_name = type_tok.value().lexeme;
    AstType type;

    if (type_name == "int") {
        type.value = AstType::Int{};
    } else if (type_name == "float") {
        type.value = AstType::Float{};
    } else if (type_name == "boolean") {
        type.value = AstType::Bool{};
    } else if (type_name == "string") {
        type.value = AstType::String{};
    } else if (type_name == "regex") {
        type.value = AstType::Regex{};
    } else if (type_name == "match") {
        type.value = AstType::Match{};
    } else if (type_name == "list") {
        auto lt = expect(TokenKind::Lt, "expected '<' after 'list'");
        if (is_err(lt)) return err<AstType>(lt.error());

        auto elem_type = parse_type();
        if (is_err(elem_type)) return err<AstType>(elem_type.error());

        auto gt = expect(TokenKind::Gt, "expected '>' after list element type");
        if (is_err(gt)) return err<AstType>(gt.error());

        type.value = AstType::List{std::make_unique<AstType>(elem_type.value())};
    } else {
        return err<AstType>(std::make_shared<Error>("unknown type: " + type_name,
                                                    type_tok.value().span, ErrorKind::Type));
    }

    return ok(type);
}

// Expression parsing (precedence climbing)
Result<Expr> Parser::parse_expression() {
    return parse_ternary();
}

Result<Expr> Parser::parse_ternary() {
    auto expr = parse_logical_or();
    if (is_err(expr)) return expr;

    if (match(TokenKind::Question)) {
        auto then_expr = parse_expression();
        if (is_err(then_expr)) return then_expr;

        auto colon = expect(TokenKind::Colon, "expected ':' in ternary expression");
        if (is_err(colon)) return err<Expr>(colon.error());

        auto else_expr = parse_expression();
        if (is_err(else_expr)) return else_expr;

        Expr result;
        result.span = Span{expr.value().span.p1, else_expr.value().span.p2};
        result.value = Expr::Ternary{std::make_unique<Expr>(std::move(expr).value()),
                                     std::make_unique<Expr>(std::move(then_expr).value()),
                                     std::make_unique<Expr>(std::move(else_expr).value())};
        return ok(result);
    }

    return expr;
}

Result<Expr> Parser::parse_logical_or() {
    auto left = parse_logical_and();
    if (is_err(left)) return left;

    while (match(TokenKind::Or)) {
        auto right = parse_logical_and();
        if (is_err(right)) return right;

        Span left_span = left.value().span;
        Span right_span = right.value().span;
        Expr result;
        result.span = Span{left_span.p1, right_span.p2};
        result.value = Expr::BinaryOp{std::make_unique<Expr>(std::move(left).value()), Operator::Or,
                                      std::make_unique<Expr>(std::move(right).value())};
        left = ok(result);
    }

    return left;
}

Result<Expr> Parser::parse_logical_and() {
    auto left = parse_comparison();
    if (is_err(left)) return left;

    while (match(TokenKind::And)) {
        auto right = parse_comparison();
        if (is_err(right)) return right;

        Span left_span = left.value().span;
        Span right_span = right.value().span;
        Expr result;
        result.span = Span{left_span.p1, right_span.p2};
        result.value = Expr::BinaryOp{std::make_unique<Expr>(std::move(left).value()), Operator::And,
                                      std::make_unique<Expr>(std::move(right).value())};
        left = ok(result);
    }

    return left;
}

Result<Expr> Parser::parse_comparison() {
    auto left = parse_addition();
    if (is_err(left)) return left;

    while (true) {
        Operator op;
        if (match(TokenKind::Eq)) {
            op = Operator::Eq;
        } else if (match(TokenKind::Ne)) {
            op = Operator::Ne;
        } else if (match(TokenKind::Gt)) {
            op = Operator::Gt;
        } else if (match(TokenKind::Lt)) {
            op = Operator::Lt;
        } else if (match(TokenKind::Ge)) {
            op = Operator::Ge;
        } else if (match(TokenKind::Le)) {
            op = Operator::Le;
        } else {
            break;
        }

        auto right = parse_addition();
        if (is_err(right)) return right;

        Span left_span = left.value().span;
        Span right_span = right.value().span;
        Expr result;
        result.span = Span{left_span.p1, right_span.p2};
        result.value = Expr::BinaryOp{std::make_unique<Expr>(std::move(left).value()), op,
                                      std::make_unique<Expr>(std::move(right).value())};
        left = ok(result);
    }

    return left;
}

Result<Expr> Parser::parse_addition() {
    auto left = parse_multiplication();
    if (is_err(left)) return left;

    while (true) {
        Operator op;
        if (match(TokenKind::Plus)) {
            op = Operator::Add;
        } else if (match(TokenKind::Minus)) {
            op = Operator::Sub;
        } else if (match(TokenKind::Concat)) {
            op = Operator::Concat;
        } else {
            break;
        }

        auto right = parse_multiplication();
        if (is_err(right)) return right;

        Span left_span = left.value().span;
        Span right_span = right.value().span;
        Expr result;
        result.span = Span{left_span.p1, right_span.p2};
        result.value = Expr::BinaryOp{std::make_unique<Expr>(std::move(left).value()), op,
                                      std::make_unique<Expr>(std::move(right).value())};
        left = ok(result);
    }

    return left;
}

Result<Expr> Parser::parse_multiplication() {
    auto left = parse_exponentiation();
    if (is_err(left)) return left;

    while (true) {
        Operator op;
        if (match(TokenKind::Star)) {
            op = Operator::Mul;
        } else if (match(TokenKind::Slash)) {
            op = Operator::Div;
        } else {
            break;
        }

        auto right = parse_exponentiation();
        if (is_err(right)) return right;

        Span left_span = left.value().span;
        Span right_span = right.value().span;
        Expr result;
        result.span = Span{left_span.p1, right_span.p2};
        result.value = Expr::BinaryOp{std::make_unique<Expr>(std::move(left).value()), op,
                                      std::make_unique<Expr>(std::move(right).value())};
        left = ok(result);
    }

    return left;
}

Result<Expr> Parser::parse_exponentiation() {
    auto left = parse_unary();
    if (is_err(left)) return left;

    if (match(TokenKind::Pow)) {
        auto right = parse_exponentiation();  // Right-associative
        if (is_err(right)) return right;

        Span left_span = left.value().span;
        Span right_span = right.value().span;
        Expr result;
        result.span = Span{left_span.p1, right_span.p2};
        result.value = Expr::BinaryOp{std::make_unique<Expr>(std::move(left).value()), Operator::Pow,
                                      std::make_unique<Expr>(std::move(right).value())};
        return ok(result);
    }

    return left;
}

Result<Expr> Parser::parse_unary() {
    if (check(TokenKind::Not)) {
        Pos start = current_.span.p1;
        advance();
        auto expr = parse_unary();
        if (is_err(expr)) return expr;

        Expr result;
        result.span = Span{start, expr.value().span.p2};
        result.value = Expr::UnaryOp{Operator::Not, std::make_unique<Expr>(std::move(expr).value())};
        return ok(result);
    }

    if (check(TokenKind::Minus)) {
        Pos start = current_.span.p1;
        advance();
        auto expr = parse_unary();
        if (is_err(expr)) return expr;

        Expr result;
        result.span = Span{start, expr.value().span.p2};
        result.value = Expr::UnaryOp{Operator::Neg, std::make_unique<Expr>(std::move(expr).value())};
        return ok(result);
    }

    return parse_primary();
}

Result<Expr> Parser::parse_primary() {
    // Literals
    if (check(TokenKind::Int)) {
        Token tok = advance();
        RuntimeValue val;
        val.value = RuntimeValue::Int{std::stoll(tok.lexeme)};
        Expr expr;
        expr.value = Expr::Literal{val};
        expr.span = tok.span;
        return ok(expr);
    }

    if (check(TokenKind::Float)) {
        Token tok = advance();
        RuntimeValue val;
        val.value = RuntimeValue::Float{std::stod(tok.lexeme)};
        Expr expr;
        expr.value = Expr::Literal{val};
        expr.span = tok.span;
        return ok(expr);
    }

    if (check(TokenKind::Bool)) {
        Token tok = advance();
        RuntimeValue val;
        val.value = RuntimeValue::Bool{tok.lexeme == "true"};
        Expr expr;
        expr.value = Expr::Literal{val};
        expr.span = tok.span;
        return ok(expr);
    }

    if (check(TokenKind::String)) {
        Token tok = advance();
        // Remove surrounding quotes
        std::string str_val = tok.lexeme;
        if (str_val.size() >= 2 && (str_val.front() == '"' || str_val.front() == '\'')) {
            str_val = str_val.substr(1, str_val.size() - 2);
        }
        RuntimeValue val;
        val.value = RuntimeValue::String{str_val};
        Expr expr;
        expr.value = Expr::Literal{val};
        expr.span = tok.span;
        return ok(expr);
    }

    if (check(TokenKind::FString)) {
        Token tok = advance();
        // Remove surrounding quotes from f-string
        std::string str_val = tok.lexeme;
        if (str_val.size() >= 2 && (str_val.front() == '"' || str_val.front() == '\'')) {
            str_val = str_val.substr(1, str_val.size() - 2);
        }
        RuntimeValue val;
        val.value = RuntimeValue::String{str_val};
        Expr expr;
        expr.value = Expr::Literal{val};
        expr.span = tok.span;
        return ok(expr);
    }

    if (check(TokenKind::Regex)) {
        Token tok = advance();
        // Parse regex: /pattern/flags
        std::string lex = tok.lexeme;
        size_t last_slash = lex.rfind('/');
        std::string pattern = lex.substr(1, last_slash - 1);
        std::string flags = (last_slash + 1 < lex.size()) ? lex.substr(last_slash + 1) : "";

        RuntimeValue val;
        val.value = RuntimeValue::Regex{RegexType{pattern, flags}};
        Expr expr;
        expr.value = Expr::Literal{val};
        expr.span = tok.span;
        return ok(expr);
    }

    // Identifier or function call or type cast
    if (check(TokenKind::Identifier)) {
        Token tok = advance();
        std::string name = tok.lexeme;

        // Check for type cast or function call
        if (check(TokenKind::LParen)) {
            Pos start = tok.span.p1;
            advance();  // consume '('

            // Check if this is a type cast (type name followed by single expression)
            bool is_type = (name == "int" || name == "float" || name == "boolean" || 
                           name == "string" || name == "regex" || name == "match" || name == "list");
            
            if (is_type) {
                // Try to parse as type cast: type(expr)
                // For list types, we need to handle list<T>(expr)
                AstType cast_type;
                if (name == "int") {
                    cast_type.value = AstType::Int{};
                } else if (name == "float") {
                    cast_type.value = AstType::Float{};
                } else if (name == "boolean") {
                    cast_type.value = AstType::Bool{};
                } else if (name == "string") {
                    cast_type.value = AstType::String{};
                } else if (name == "regex") {
                    cast_type.value = AstType::Regex{};
                } else if (name == "match") {
                    cast_type.value = AstType::Match{};
                } else if (name == "list") {
                    // list<T>(expr) - need to parse the type parameter
                    auto lt = expect(TokenKind::Lt, "expected '<' after 'list' in type cast");
                    if (is_err(lt)) return err<Expr>(lt.error());
                    
                    auto elem_type = parse_type();
                    if (is_err(elem_type)) return err<Expr>(elem_type.error());
                    
                    auto gt = expect(TokenKind::Gt, "expected '>' after list element type");
                    if (is_err(gt)) return err<Expr>(gt.error());
                    
                    cast_type.value = AstType::List{std::make_unique<AstType>(std::move(elem_type).value())};
                    
                    auto lparen2 = expect(TokenKind::LParen, "expected '(' after list type");
                    if (is_err(lparen2)) return err<Expr>(lparen2.error());
                }

                // Parse the expression to cast
                auto cast_expr = parse_expression();
                if (is_err(cast_expr)) return err<Expr>(cast_expr.error());

                auto rparen = expect(TokenKind::RParen, "expected ')' after type cast expression");
                if (is_err(rparen)) return err<Expr>(rparen.error());

                Expr expr;
                expr.span = Span{start, rparen.value().span.p2};
                expr.value = Expr::TypeCast{cast_type, std::make_unique<Expr>(std::move(cast_expr).value())};
                return ok(expr);
            }

            // Regular function call
            std::vector<ExprPtr> args;
            if (!check(TokenKind::RParen)) {
                do {
                    auto arg = parse_expression();
                    if (is_err(arg)) return arg;
                    args.push_back(std::make_unique<Expr>(arg.value()));
                } while (match(TokenKind::Comma));
            }

            auto rparen = expect(TokenKind::RParen, "expected ')' after function arguments");
            if (is_err(rparen)) return err<Expr>(rparen.error());

            Expr expr;
            expr.span = Span{tok.span.p1, rparen.value().span.p2};
            expr.value = Expr::FunctionCall{name, std::move(args)};
            return ok(expr);
        }

        // Just a variable
        Expr expr;
        expr.value = Expr::Variable{name};
        expr.span = tok.span;
        return ok(expr);
    }

    // Parenthesized expression
    if (match(TokenKind::LParen)) {
        auto expr = parse_expression();
        if (is_err(expr)) return expr;

        auto rparen = expect(TokenKind::RParen, "expected ')' after expression");
        if (is_err(rparen)) return err<Expr>(rparen.error());

        return expr;
    }

    // List literal: {expr, expr, ...}
    if (check(TokenKind::LBrace)) {
        Pos start = current_.span.p1;
        advance();  // consume '{'

        std::vector<ExprPtr> elements;
        if (!check(TokenKind::RBrace)) {
            do {
                auto elem = parse_expression();
                if (is_err(elem)) return elem;
                elements.push_back(std::make_unique<Expr>(std::move(elem).value()));
            } while (match(TokenKind::Comma));
        }

        auto rbrace = expect(TokenKind::RBrace, "expected '}' after list elements");
        if (is_err(rbrace)) return err<Expr>(rbrace.error());

        Expr expr;
        expr.span = Span{start, rbrace.value().span.p2};
        expr.value = Expr::ListLiteral{std::move(elements)};
        return ok(expr);
    }

    return err<Expr>(std::make_shared<Error>("unexpected token in expression", current_.span,
                                             ErrorKind::Syntax));
}

}  // namespace parser
