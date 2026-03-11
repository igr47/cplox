#ifndef LOX_COMPILER_HPP
#define LOX_COMPILER_HPP

#include "token.h"
#include "chunk.h"
#include <string_view>
#include <functional>
#include <unordered_map>

// Precedence levels (from lowest to highest)
enum class Precedence {
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR, // or
    PREC_AND, //and
    PREC_EQUALITY, // == !=
    PREC_COMPARISON, // <> <= >=
    PREC_TERM, // + -
    PREC_FACTOR, // * /
    PREC_UNARY, // ! -
    PREC_CALL, // . ()
    PREC_PRIMARY
};

// Foward declaration
class Parser;

// Parse function type
using ParseFn = void (*)(Parser* parser, bool canAssign);

// Parse rule structure
struct ParseRule {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;

    ParseRule() : prefix(nullptr), infix(nullptr), precedence(Precedence::PREC_NONE) {}
    ParseRule(ParseFn p, ParseFn i, Precedence prec) : prefix(p), infix(i), precedence(prec) {}
};

class Parser {
    public:
        Token current;
        Token previous;
        bool hadError = false;
        bool panicMode = false;
        Chunk* compilingChunk = nullptr;

        // Parse rule table
        static std::unordered_map<TokenType, ParseRule> rules;

        Parser() = default;
        ~Parser() = default;

        // Error handling
        void errorAtCurrent(const std::string& message);
        void error(const std::string& message);
        void errorAt(const Token* token, const std::string& message);

        // Parsing utilities
        void advance();
        void consume(TokenType type, const std::string& message);
        bool check(TokenType type) const;
        bool match(TokenType type);

        // Bytecode emission
        void emitByte(uint8_t byte);
        void emitBytes(uint8_t byte1, uint8_t byte2);
        void emitReturn();
        uint8_t makeConstant(Value value);
        void emitConstant(Value value);

        // Expression parsing
        void expression();
        void parsePrecedence(Precedence precedence);
        ParseRule* getRule(TokenType type);
};

class Compiler {
    public:
        Compiler() = default;
        ~Compiler() = default;

        // Compile source code
        bool compile(std::string_view source, Chunk* chunk);
};

// Expression parsing functions
void number(Parser* parser, bool canAssign);
void grouping(Parser* parser, bool canAssign);
void unary(Parser* parser, bool canAssign);
void binary(Parser* parser, bool canAssign);

#endif
