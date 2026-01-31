// lexer.cpp
// Implements lexer.h

#include "lexer.h"

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
            next_char();  // /
            next_char();  // /
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
    Token t{is_float ? TokenKind::Float : TokenKind::Int, std::string(lex), Span{start, end}};
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
    return Token{kind, std::string(lex), Span{start, end}};
}

Token Lexer::read_string(Pos start) {
    char quote = peek(0);
    // consume opening quote
    next_char();
    std::string lex;
    lex.push_back(quote);

    while (true) {
        char c = peek(0);
        if (c == '\0') break;  // unterminated handled by caller via Result

        if (c == '\\') {
            // consume backslash
            next_char();
            char n = peek(0);
            if (n == '\0') break;
            // collapse backslash + newline (including CRLF) -> remove both
            if (n == '\n') {
                next_char();  // consume newline
                continue;
            }
            if (n == '\r') {
                next_char();                       // consume CR
                if (peek(0) == '\n') next_char();  // consume LF if present
                continue;
            }
            // preserve other escapes (keep backslash and escaped char)
            lex.push_back('\\');
            lex.push_back(n);
            next_char();  // consume the escaped char
            continue;
        }

        if (c == quote) {
            next_char();
            lex.push_back(quote);
            break;
        }
        // normal char
        lex.push_back(c);
        next_char();
    }

    Pos end{line_, column_};
    return Token{TokenKind::String, std::move(lex), Span{start, end}};
}

Token Lexer::read_regex(Pos start) {
    // assume leading '/'
    size_t start_idx = pos_;
    next_char();  // consume '/'
    while (true) {
        char c = peek(0);
        if (c == '\0') break;
        if (c == '\\') {
            next_char();
            if (peek(0) != '\0') next_char();
            continue;
        }
        if (c == '/') {
            next_char();
            break;
        }
        next_char();
    }
    // flags
    while (std::isalpha(static_cast<unsigned char>(peek(0)))) next_char();

    size_t len = pos_ - start_idx;
    auto lex = src_.substr(start_idx, len);
    Pos end{line_, column_};
    return Token{TokenKind::Regex, std::string(lex), Span{start, end}};
}

Token Lexer::read_operator_or_punct(Pos start) {
    char c = peek(0);
    char n = peek(1);
    size_t start_idx = pos_;

    // multi-char tokens
    if (c == '=' && n == '=') {
        next_char();
        next_char();
        return Token{TokenKind::Eq, std::string(src_.substr(start_idx, 2)),
                     Span{start, Pos{line_, column_}}};
    }
    if (c == '!' && n == '=') {
        next_char();
        next_char();
        return Token{TokenKind::Ne, std::string(src_.substr(start_idx, 2)),
                     Span{start, Pos{line_, column_}}};
    }
    if (c == '>' && n == '=') {
        next_char();
        next_char();
        return Token{TokenKind::Ge, std::string(src_.substr(start_idx, 2)),
                     Span{start, Pos{line_, column_}}};
    }
    if (c == '<' && n == '=') {
        next_char();
        next_char();
        return Token{TokenKind::Le, std::string(src_.substr(start_idx, 2)),
                     Span{start, Pos{line_, column_}}};
    }
    if (c == '&' && n == '&') {
        next_char();
        next_char();
        return Token{TokenKind::And, std::string(src_.substr(start_idx, 2)),
                     Span{start, Pos{line_, column_}}};
    }
    if (c == '|' && n == '|') {
        next_char();
        next_char();
        return Token{TokenKind::Or, std::string(src_.substr(start_idx, 2)),
                     Span{start, Pos{line_, column_}}};
    }
    if (c == '*' && n == '*') {
        next_char();
        next_char();
        return Token{TokenKind::Pow, std::string(src_.substr(start_idx, 2)),
                     Span{start, Pos{line_, column_}}};
    }
    if (c == '+' && n == '+') {
        next_char();
        next_char();
        return Token{TokenKind::Concat, std::string(src_.substr(start_idx, 2)),
                     Span{start, Pos{line_, column_}}};
    }

    // single char
    next_char();
    Pos end{line_, column_};
    auto lex = src_.substr(start_idx, 1);
    switch (c) {
        case '(':
            return Token{TokenKind::LParen, std::string(lex), Span{start, end}};
        case ')':
            return Token{TokenKind::RParen, std::string(lex), Span{start, end}};
        case '{':
            return Token{TokenKind::LBrace, std::string(lex), Span{start, end}};
        case '}':
            return Token{TokenKind::RBrace, std::string(lex), Span{start, end}};
        case '[':
            return Token{TokenKind::LBracket, std::string(lex), Span{start, end}};
        case ']':
            return Token{TokenKind::RBracket, std::string(lex), Span{start, end}};
        case ',':
            return Token{TokenKind::Comma, std::string(lex), Span{start, end}};
        case ';':
            return Token{TokenKind::Semicolon, std::string(lex), Span{start, end}};
        case '+':
            return Token{TokenKind::Plus, std::string(lex), Span{start, end}};
        case '-':
            return Token{TokenKind::Minus, std::string(lex), Span{start, end}};
        case '*':
            return Token{TokenKind::Star, std::string(lex), Span{start, end}};
        case '/':
            return Token{TokenKind::Slash, std::string(lex), Span{start, end}};
        case '=':
            return Token{TokenKind::Assign, std::string(lex), Span{start, end}};
        case '>':
            return Token{TokenKind::Gt, std::string(lex), Span{start, end}};
        case '<':
            return Token{TokenKind::Lt, std::string(lex), Span{start, end}};
        case '!':
            return Token{TokenKind::Not, std::string(lex), Span{start, end}};
        case '?':
            return Token{TokenKind::Question, std::string(lex), Span{start, end}};
        case ':':
            return Token{TokenKind::Colon, std::string(lex), Span{start, end}};
        default:
            return Token{TokenKind::Unknown, std::string(lex), Span{start, end}};
    }
}

Result<Token> Lexer::next_token() {
    skip_whitespace_and_comments();

    if (eof()) {
        Token t{TokenKind::EndOfFile, std::string(src_.substr(pos_, 0)),
                Span{Pos{line_, column_}, Pos{line_, column_}}};
        return ok(t);
    }

    Pos start{line_, column_};

    char c = peek(0);

    // helper that records last token kind and returns Result<Token>
    auto emit = [&](Token t) -> Result<Token> {
        this->last_token_kind_ = t.kind;
        return ok(std::move(t));
    };

    // support f-strings: an 'f' immediately followed by a quote
    if (c == 'f' && (peek(1) == '"' || peek(1) == '\'')) {
        // consume the 'f' prefix so read_string consumes the quote
        next_char();
        Token t = read_string(start);
        // prepend 'f' to lexeme so parser/runtime can detect it if needed
        t.lexeme.insert(t.lexeme.begin(), 'f');
        return emit(std::move(t));
    }

    if (std::isdigit(static_cast<unsigned char>(c))) {
        Token t = read_number(start);
        return emit(std::move(t));
    }

    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        Token t = read_identifier_or_keyword(start);
        return emit(std::move(t));
    }

    if (c == '"' || c == '\'') {
        Token t = read_string(start);
        // detect unterminated crude check: must end with same quote
        if (t.lexeme.size() < 2 || (t.lexeme.front() != '"' && t.lexeme.front() != '\'')) {
            Token dummy{TokenKind::Unknown, t.lexeme, Span{start, Pos{line_, column_}}};
            return err<Token>(
                std::make_shared<Error>("Unterminated string literal", ErrorKind::Syntax));
        }
        return emit(std::move(t));
    }

    // Regex or slash/operator: use context heuristic
    if (c == '/' && peek(1) != '/') {
        auto ends_expr = [](TokenKind k) -> bool {
            switch (k) {
                case TokenKind::Identifier:
                case TokenKind::Int:
                case TokenKind::Float:
                case TokenKind::String:
                case TokenKind::Bool:
                case TokenKind::Regex:
                case TokenKind::RParen:
                case TokenKind::RBracket:
                case TokenKind::RBrace:
                case TokenKind::EndOfFile:
                    return true;
                default:
                    return false;
            }
        };

        if (ends_expr(this->last_token_kind_)) {
            Token op = read_operator_or_punct(start);
            return emit(std::move(op));
        }

        // Try a quick forward scan for an unescaped closing '/'
        size_t scan_i = pos_ + 1;
        bool found = false;
        const size_t src_sz = src_.size();
        while (scan_i < src_sz) {
            char cc = src_[scan_i];
            if (cc == '\\') {
                scan_i += 2;
                continue;
            }
            if (cc == '/') {
                found = true;
                ++scan_i;  // consume flags
                while (scan_i < src_sz && std::isalpha(static_cast<unsigned char>(src_[scan_i])))
                    ++scan_i;
                break;
            }
            ++scan_i;
        }

        if (found) {
            Token t = read_regex(start);
            if (t.lexeme.size() < 2 || t.lexeme.front() != '/') {
                Token dummy{TokenKind::Unknown, t.lexeme, Span{start, Pos{line_, column_}}};
                return err<Token>(
                    std::make_shared<Error>("Unterminated regex literal", ErrorKind::Syntax));
            }
            return emit(std::move(t));
        }

        Token op = read_operator_or_punct(start);
        return emit(std::move(op));
    }

    // otherwise operators/punct
    Token op = read_operator_or_punct(start);
    return emit(std::move(op));
}

}  // namespace lexer
