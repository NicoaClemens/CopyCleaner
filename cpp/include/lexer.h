// lexer.h

#pragma once

#include <string>
#include <string_view>

#include "ast.h"
#include "errors.hpp"

namespace lexer {

enum class TokenKind {
    Identifier, Int, Float, String, Bool, Regex,
    KwFunction, KwReturns, KwIf, KwElif, KwElse,
    KwWhile, KwReturn, KwBreak, KwContinue,
    LParen, RParen, LBrace, RBrace, LBracket, RBracket,
    Comma, Semicolon, Plus, Minus, Star, Slash, Pow, Concat,
    Eq, Assign, Ne, Gt, Lt, Ge, Le, And, Or, Not,
    Question, Colon, EndOfFile, Unknown
};


struct Token {
    TokenKind kind;
    std::string_view lexeme; // raw text
    Span span;

    std::string copy_lexeme() const { return std::string(lexeme); }
};

class Lexer {
public:
    Lexer(std::string_view source);

    Result<Token> next_token();
    bool eof() const;
private:
    char peek(size_t offset = 0) const;
    char next_char();
    void skip_whitespace_and_comments();
    Token read_number(Pos start);
    Token read_operator_or_punct(Pos start);
    Token read_identifier_or_keyword(Pos start);
    Token read_string(Pos start);
    Token read_regex(Pos start);

    std::string_view src_;
    size_t pos_ = 0;
    size_t line_ = 1;
    size_t column_ = 1;
};

} // namespace lexer