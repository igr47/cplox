#ifndef LOX_TOKEN_HPP
#define LOX_TOKEN_HPP

#include <string>
#include <string_view>

enum class TokenType {
    // Single-charachter tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE,RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two charachter tokens
    BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL, GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    AND, CLASS, ELSE, FALSE, FOR, FUN, IF, NIL, OR, PRINT,
    RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    // Special
    ERROR, TOKEN_EOF
};

struct Token {
    TokenType type;
    std::string_view lexeme;
    int line;

    Token() : type(TokenType::TOKEN_EOF), line(0) {}

    Token(TokenType type, std::string_view lexeme, int line) : type(type), lexeme(lexeme), line(line) {}
};

#endif
