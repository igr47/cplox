#ifndef LOX_SCANNER_HPP
#define LOX_SCANNER_HPP

#include "token.h"
#include <string>
#include <string_view>

class Scanner {
    public:
        explicit Scanner(std::string_view source);

        // Scan the next token
        Token scanToken();

    private:
        std::string_view source;
        const char* start;
        const char* current;
        int line;

        // Helper methods
        bool isAtEnd() const;
        char advance();
        char peek() const;
        char peekNext() const;
        bool match(char expected);
        void skipWhitespace();

        //Token creation helpers
        Token makeToken(TokenType type);
        Token errorToken(const std::string& message);

        // Literal Scanners
        Token string();
        Token number();
        Token identifier();

        // Keyword hndling
        TokenType identifierType();
        TokenType checkKeyword(int start, int length, const std::string& rest, TokenType type) const;

        // Charachter classification
        static bool isDigit(char c);
        static bool isAlpha(char c);
        static bool isAlphaNumeric(char c);
};

#endif
