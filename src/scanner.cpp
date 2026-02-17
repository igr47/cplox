#include "scanner.h"
#include <cstring>

Scanner::Scanner(std::string_view source) :
    source(source),
    start(source.data()),
    current(source.data()),
    line(1) {}

bool Scanner::isAtEnd() const {
    return *current == '\0';
}

char Scanner::advance() {
    current++;
    return current[-1];
}

char Scanner::peek() const {
    return *current;
}

char Scanner::peekNext() const {
    if (isAtEnd()) return '\0';
    return current[1];
}

bool Scanner::match(char expected) {
    if (isAtEnd()) return false;
    if (*current != expected) return false;

    current++;
    return true;
}

void Scanner::skipWhitespace() {
    for(;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                advance();
                break;

            case '/':
                if (peekNext() == '/') {
                    // Comment goes until end of line
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;

            default:
                return;
        }
    }
}

Token Scanner::makeToken(TokenType type) {
    return Token(type, std::string_view(start, current - start), line);
}

Token Scanner::scanToken() {
    skipWhitespace();
    start = current;

    if (isAtEnd()) return makeToken(TokenType::TOKEN_EOF);

    char c = advance();

    // Handle single-charachter tokens
    switch (c) {
        case '(': return makeToken(TokenType::LEFT_PAREN);
        case ')': return makeToken(TokenType::RIGHT_PAREN);
        case '{': return makeToken(TokenType::LEFT_BRACE);
        case '}': return makeToken(TokenType::RIGHT_BRACE);
        case ';': return makeToken(TokenType::SEMICOLON);
        case ',': return makeToken(TokenType::COMMA);
        case '.': return makeToken(TokenType::DOT);
        case '-': return makeToken(TokenType::MINUS);
        case '+': return makeToken(TokenType::PLUS);
        case '/': return makeToken(TokenType::SLASH);
        case '*': return makeToken(TokenType::STAR);

        // Two charachter tokens
        case '!':
            return makeToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        case '=':
            return makeToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case '<':
            return makeToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>':
            return makeToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);

        // Literals
        case '"': return string();
    }

    // Handle numbers, identifiers and keywords
    if (isDigit(c)) return number();
    if (isAlpha(c)) return identifier();

    return errorToken("Unexpected charachter.");
}

Token Scanner::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated String.");
    // The closing quote
    advance();
    return makeToken(TokenType::STRING);
}

bool Scanner::isDigit(char c) {
    return c >= '0' && c <= '9';
}

Token Scanner::number() {
    while (isDigit(peek())) advance();

    // Look for fractional part
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the '.'
        advance();
        while (isDigit(peek())) advance();
    }

    return makeToken(TokenType::NUMBER);
}

bool Scanner::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

Token Scanner::identifier() {
    while (isAlphaNumeric(peek())) advance();
    return makeToken(identifierType());
}

TokenType Scanner::checkKeyword(int startPos, int length, const std::string& rest, TokenType type) const {
    if (static_cast<int>(current - start) == startPos + length && std::memcmp(start + startPos, rest.c_str(), length) == 0) {
        return type;
    }
    return TokenType::IDENTIFIER;
}

TokenType Scanner::identifierType() {
    // Check fisrt charachter
    switch (start[0]) {
        case 'a': return checkKeyword(1,2, "nd", TokenType::AND);
        case 'c': return checkKeyword(1,4, "lass", TokenType::CLASS);
        case 'e': return checkKeyword(1,3, "lse", TokenType::ELSE);
        case 'f': {
            if (current - start > 1) {
                switch (start[1]) {
                    case 'a': return checkKeyword(2,3, "lse", TokenType::FALSE);
                    case 'o': return checkKeyword(2,1, "r", TokenType::FOR);
                    case 'u': return checkKeyword(2,2, "n", TokenType::FUN);
                }
            }
            break;
        }
        case 'i': return checkKeyword(1, 1, "f", TokenType::IF);
        case 'n': return checkKeyword(1, 2, "il", TokenType::NIL);
        case 'o': return checkKeyword(1, 1, "r", TokenType::OR);
        case 'p': return checkKeyword(1, 4, "rint", TokenType::PRINT);
        case 'r': return checkKeyword(1, 5, "eturn", TokenType::RETURN);
        case 's': return checkKeyword(1, 4, "uper", TokenType::SUPER);
        case 't': {
            if (current - start > 1) {
                switch (start[1]) {
                    case 'h': return checkKeyword(2, 2, "is", TokenType::THIS);
                    case 'r': return checkKeyword(2, 2, "ue", TokenType::TRUE);
                }
            }
            break;
        }
        case 'v': return checkKeyword(1, 2, "ar", TokenType::VAR);
        case 'w': return checkKeyword(1, 4, "hile", TokenType::WHILE);
    }

    return TokenType::IDENTIFIER;
}

