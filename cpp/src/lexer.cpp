// lexer.cpp

#include "../include/lexer.h"

#include <cctype>
#include <string>
#include <unordered_map>

namespace lexer {

Lexer::Lexer(std::string_view source) : src_(source) {}

bool Lexer::eof() const {
    return pos_ >= src_.size();
}

char Lexer::peek(size_t offset) const {
    size_t idx = pos_ + offset;
    if (idx >= src_.size()) return '\0';
    return src_[idx];
}

char Lexer::next_char() {
    char c = peek(0);
    if (c == '\0') return '\0';
    pos_++;
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

void Lexer::skip_whitespace_and_comments() {
    while (true) {
        char c = peek(0);
        // whitespace
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            next_char();
            continue;
        }

        // comment //
        if (c == '/' && peek(1) == '/') {
            next_char(); // /
            next_char(); // /
            while (peek(0) != '\n' && peek(0) != '\0') next_char();
            continue;
        }
        break;
    }
}

Token Lexer::read_number(Pos start) {
    size_t start_idx = pos_;
    bool is_float = false;

    while (std::isdigit(static_cast<unsigned char>(peek(0)))) next_char();

    if (peek(0) == '.' && std::isdigit(static_cast<unsigned char>(peek(1)))) {
        is_float = true;
        next_char();
        while (std::isdigit(static_cast<unsigned char>(peek(0)))) next_char();
    }

    if (peek(0) == 'e' || peek(0) == 'E') {
        is_float = true;
        next_char();
        if (peek(0) == '+' || peek(0) == '-') next_char();
        while (std::isdigit(static_cast<unsigned char>(peek(0)))) next_char();
    }

    size_t len = pos_ - start_idx;
    auto lex = src_.substr(start_idx, len);
    Pos end{line_, column_};
    Token t{ is_float ? TokenKind::Float : TokenKind::Int, lex, Span{start, end} };
    return t;
}

Token Lexer::read_identifier_or_keyword(Pos start) {
    size_t start_idx = pos_;
    while (std::isalnum(static_cast<unsigned char>(peek(0))) || peek(0) == '_') next_char();
    size_t len = pos_ - start_idx;
    auto lex = src_.substr(start_idx, len);
    std::string s(lex);

    static const std::unordered_map<std::string, TokenKind> kw = {
        {"function", TokenKind::KwFunction},
        {"returns", TokenKind::KwReturns},
        {"if", TokenKind::KwIf},
        {"elif", TokenKind::KwElif},
        {"else", TokenKind::KwElse},
        {"while", TokenKind::KwWhile},
        {"return", TokenKind::KwReturn},
        {"break", TokenKind::KwBreak},
        {"continue", TokenKind::KwContinue},
        {"true", TokenKind::Bool},
        {"false", TokenKind::Bool},
    };

    auto it = kw.find(s);
    TokenKind kind = (it != kw.end()) ? it->second : TokenKind::Identifier;
    Pos end{line_, column_};
    return Token{kind, lex, Span{start, end}};
}

Token Lexer::read_string(Pos start) {
    char quote = peek(0);
    size_t start_idx = pos_;
    next_char(); // consume opening quote
    while (true) {
        char c = peek(0);
        if (c == '\0') break; // unterminated handled by caller via Result
        if (c == '\\') {
            next_char(); // consume backslash
            if (peek(0) != '\0') next_char();
            continue;
        }
        if (c == quote) { next_char(); break; }
        next_char();
    }
    size_t len = pos_ - start_idx;
    auto lex = src_.substr(start_idx, len);
    Pos end{line_, column_};
    return Token{TokenKind::String, lex, Span{start, end}};
}

Token Lexer::read_regex(Pos start) {
    // assume leading '/'
    size_t start_idx = pos_;
    next_char(); // consume '/'
    while (true) {
        char c = peek(0);
        if (c == '\0') break;
        if (c == '\\') {
            next_char();
            if (peek(0) != '\0') next_char();
            continue;
        }
        if (c == '/') { next_char(); break; }
        next_char();
    }
    // flags
    while (std::isalpha(static_cast<unsigned char>(peek(0)))) next_char();

    size_t len = pos_ - start_idx;
    auto lex = src_.substr(start_idx, len);
    Pos end{line_, column_};
    return Token{TokenKind::Regex, lex, Span{start, end}};
}

Token Lexer::read_operator_or_punct(Pos start) {
    char c = peek(0);
    char n = peek(1);
    size_t start_idx = pos_;

    // multi-char tokens
    if (c == '=' && n == '=') { next_char(); next_char(); return Token{TokenKind::Eq, src_.substr(start_idx,2), Span{start, Pos{line_,column_}}}; }
    if (c == '!' && n == '=') { next_char(); next_char(); return Token{TokenKind::Ne, src_.substr(start_idx,2), Span{start, Pos{line_,column_}}}; }
    if (c == '>' && n == '=') { next_char(); next_char(); return Token{TokenKind::Ge, src_.substr(start_idx,2), Span{start, Pos{line_,column_}}}; }
    if (c == '<' && n == '=') { next_char(); next_char(); return Token{TokenKind::Le, src_.substr(start_idx,2), Span{start, Pos{line_,column_}}}; }
    if (c == '&' && n == '&') { next_char(); next_char(); return Token{TokenKind::And, src_.substr(start_idx,2), Span{start, Pos{line_,column_}}}; }
    if (c == '|' && n == '|') { next_char(); next_char(); return Token{TokenKind::Or, src_.substr(start_idx,2), Span{start, Pos{line_,column_}}}; }
    if (c == '*' && n == '*') { next_char(); next_char(); return Token{TokenKind::Pow, src_.substr(start_idx,2), Span{start, Pos{line_,column_}}}; }
    if (c == '+' && n == '+') { next_char(); next_char(); return Token{TokenKind::Concat, src_.substr(start_idx,2), Span{start, Pos{line_,column_}}}; }

    // single char
    next_char();
    Pos end{line_, column_};
    auto lex = src_.substr(start_idx, 1);
    switch (c) {
        case '(' : return Token{TokenKind::LParen, lex, Span{start,end}};
        case ')' : return Token{TokenKind::RParen, lex, Span{start,end}};
        case '{' : return Token{TokenKind::LBrace, lex, Span{start,end}};
        case '}' : return Token{TokenKind::RBrace, lex, Span{start,end}};
        case '[' : return Token{TokenKind::LBracket, lex, Span{start,end}};
        case ']' : return Token{TokenKind::RBracket, lex, Span{start,end}};
        case ',' : return Token{TokenKind::Comma, lex, Span{start,end}};
        case ';' : return Token{TokenKind::Semicolon, lex, Span{start,end}};
        case '+' : return Token{TokenKind::Plus, lex, Span{start,end}};
        case '-' : return Token{TokenKind::Minus, lex, Span{start,end}};
        case '*' : return Token{TokenKind::Star, lex, Span{start,end}};
        case '/' : return Token{TokenKind::Slash, lex, Span{start,end}};
        case '=' : return Token{TokenKind::Assign, lex, Span{start,end}};
        case '>' : return Token{TokenKind::Gt, lex, Span{start,end}};
        case '<' : return Token{TokenKind::Lt, lex, Span{start,end}};
        case '!' : return Token{TokenKind::Not, lex, Span{start,end}};
        case '?' : return Token{TokenKind::Question, lex, Span{start,end}};
        case ':' : return Token{TokenKind::Colon, lex, Span{start,end}};
        default: return Token{TokenKind::Unknown, lex, Span{start,end}};
    }
}

Result<Token> Lexer::next_token() {
    skip_whitespace_and_comments();

    if (eof()) {
        Token t{TokenKind::EndOfFile, src_.substr(pos_,0), Span{Pos{line_,column_}, Pos{line_,column_}}};
        return ok(t);
    }

    Pos start{line_, column_};

    char c = peek(0);

    if (std::isdigit(static_cast<unsigned char>(c))) {
        Token t = read_number(start);
        return ok(t);
    }

    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        Token t = read_identifier_or_keyword(start);
        return ok(t);
    }

    if (c == '"' || c == '\'') {
        Token t = read_string(start);
        // detect unterminated crude check: must end with same quote
        if (t.lexeme.size() < 2 || (t.lexeme.front() != '"' && t.lexeme.front() != '\'')) {
            Token dummy{TokenKind::Unknown, t.lexeme, Span{start, Pos{line_,column_}}};
            return err<Token>(std::make_shared<Error>("Unterminated string literal", ErrorKind::Syntax));
        }
        return ok(t);
    }

    // Regex or slash/operator
    if (c == '/' && peek(1) != '/') {
        Token t = read_regex(start);
        // crude unterminated check: regex lexeme must contain at least one '/'
        if (t.lexeme.size() < 2 || t.lexeme.front() != '/') {
            Token dummy{TokenKind::Unknown, t.lexeme, Span{start, Pos{line_,column_}}};
            return err<Token>(std::make_shared<Error>("Unterminated regex literal", ErrorKind::Syntax));
        }
        return ok(t);
    }

    // otherwise operators/punct
    Token op = read_operator_or_punct(start);
    return ok(op);
}

} // namespace lexer
