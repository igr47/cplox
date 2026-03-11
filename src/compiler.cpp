#include "compiler.h"
#include "scanner.h"
//#include "debug.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <unordered_map>

static int addConstant(Chunk* chunk, Value value) {
    chunk->writeConstant(value);
    return chunk->constants.values.size() -1;
}

// Initialize parse rules table
std::unordered_map<TokenType, ParseRule> Parser::rules = {
    {TokenType::LEFT_PAREN,   ParseRule(grouping, nullptr, Precedence::PREC_NONE)},
    {TokenType::RIGHT_PAREN,  ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::LEFT_BRACE,   ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::RIGHT_BRACE,  ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::COMMA,        ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::DOT,          ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::MINUS,        ParseRule(unary, binary, Precedence::PREC_TERM)},
    {TokenType::PLUS,         ParseRule(nullptr, binary, Precedence::PREC_TERM)},
    {TokenType::SEMICOLON,    ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::SLASH,        ParseRule(nullptr, binary, Precedence::PREC_FACTOR)},
    {TokenType::STAR,         ParseRule(nullptr, binary, Precedence::PREC_FACTOR)},
    {TokenType::BANG,         ParseRule(unary, nullptr, Precedence::PREC_NONE)},
    {TokenType::BANG_EQUAL,   ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::EQUAL,        ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::EQUAL_EQUAL,  ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::GREATER,      ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::GREATER_EQUAL,ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::LESS,         ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::LESS_EQUAL,   ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::IDENTIFIER,   ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::STRING,       ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::NUMBER,       ParseRule(number, nullptr, Precedence::PREC_NONE)},
    {TokenType::AND,          ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::CLASS,        ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::ELSE,         ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::FALSE,        ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::FOR,          ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::FUN,          ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::IF,           ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::NIL,          ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::OR,           ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::PRINT,        ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::RETURN,       ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::SUPER,        ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::THIS,         ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::TRUE,         ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::VAR,          ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::WHILE,        ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::ERROR,        ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
    {TokenType::TOKEN_EOF,    ParseRule(nullptr, nullptr, Precedence::PREC_NONE)},
};

// Error handling
void Parser::errorAt(const Token* token, const std::string& message) {
    if (panicMode) return;
    panicMode = true;

    std::fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TokenType::TOKEN_EOF) {
        std::fprintf(stderr, " at end");
    } else if (token->type == TokenType::ERROR) {
        // Nothing
    } else {
        std::fprintf(stderr, " at '%.*s'", static_cast<int>(token->lexeme.length()), token->lexeme.data());
    }

    std::fprintf(stderr, ": %s\n", message.c_str());
    hadError = true;
}

void Parser::errorAtCurrent(const std::string& message) {
    errorAt(&current, message);
}

void Parser::error(const std::string& message) {
    errorAt(&previous, message);
}

void Parser::advance() {
    previous = current;

    for (;;) {
        //Scanner scanner(compilingChunk ? "" : "");
        if (!scanner) {
            error("No scanner available");
            return;
        }
        current = scanner->scanToken();

        if (current.type != TokenType::ERROR) break;

        errorAtCurrent(std::string(current.lexeme.data(), current.lexeme.length()));
    }
}

void Parser::consume(TokenType type, const std::string& message) {
    if (current.type == type) {
        advance();
        return;
    }

    errorAtCurrent(message);
}

bool Parser::check(TokenType type) const {
    return current.type == type;
}

bool Parser::match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

// Bytecode emision
void Parser::emitByte(uint8_t byte) {
    if (compilingChunk) {
        compilingChunk->write(byte, previous.line);
    }
}

void Parser::emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

void Parser::emitReturn() {
    emitByte(static_cast<uint8_t>(OpCode::OP_RETURN));
}

uint8_t Parser::makeConstant(Value value) {
    int constant = addConstant(compilingChunk, value);
    if (constant > UINT8_MAX) {
        error("Too many conatants in one chunk.");
        return 0;
    }
    return static_cast<uint8_t>(constant);
}

void Parser::emitConstant(Value value) {
    emitBytes(static_cast<uint8_t>(OpCode::OP_CONSTANT), makeConstant(value));
}

// Expression parsing functions
void number(Parser* parser, bool canAssign) {
    double value = std::strtod(std::string(parser->previous.lexeme).c_str(), nullptr);
    parser->emitConstant(value);
}

void grouping(Parser* parser, bool canAssign) {
    parser->expression();
    parser->consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
}

void unary(Parser* parser, bool canAssign) {
    TokenType operatorType = parser->previous.type;

    // Compile the operand
    parser->parsePrecedence(Precedence::PREC_UNARY);

    // Emit the operator instruction
    switch (operatorType) {
        case TokenType::MINUS:
            parser->emitByte(static_cast<uint8_t>(OpCode::OP_NEGATE));
            break;
        case TokenType::BANG:
            // We'll handle this in a future chapter
            break;
        default:
            return; // Unreachable
    }
}

void binary(Parser* parser, bool canAssign) {
    // Remember the operator
    TokenType operatorType = parser->previous.type;

    // Compile the right operand
    ParseRule* rule = parser->getRule(operatorType);
    if (rule) {
        parser->parsePrecedence(static_cast<Precedence>(
            static_cast<int>(rule->precedence) + 1));
    }

    // Emit the operator instruction
    switch (operatorType) {
        case TokenType::PLUS:
            parser->emitByte(static_cast<uint8_t>(OpCode::OP_ADD));
            break;
        case TokenType::MINUS:
            parser->emitByte(static_cast<uint8_t>(OpCode::OP_SUBTRACT));
            break;
        case TokenType::STAR:
            parser->emitByte(static_cast<uint8_t>(OpCode::OP_MULTIPLY));
            break;
        case TokenType::SLASH:
            parser->emitByte(static_cast<uint8_t>(OpCode::OP_DIVIDE));
            break;
        default:
            return; // Unreachable
    }
}

// Main parsing functions
ParseRule* Parser::getRule(TokenType type) {
    auto it = rules.find(type);
    if (it != rules.end()) {
        return &it->second;
    }
    return nullptr;
}

void Parser::parsePrecedence(Precedence precedence) {
    advance();

    ParseFn prefixRule = getRule(previous.type)->prefix;
    if (prefixRule == nullptr) {
        error("Expect expression.");
        return;
    }

    bool canAssign = precedence <= Precedence::PREC_ASSIGNMENT;
    prefixRule(this, canAssign);

    while (static_cast<int>(precedence) <=
           static_cast<int>(getRule(current.type)->precedence)) {
        advance();
        ParseFn infixRule = getRule(previous.type)->infix;
        if (infixRule) {
            infixRule(this, canAssign);
        }
    }

    if (canAssign && match(TokenType::EQUAL)) {
        error("Invalid assignment target.");
    }
}

void Parser::expression() {
    parsePrecedence(Precedence::PREC_ASSIGNMENT);
}

// Compiler main function
bool Compiler::compile(std::string_view source, Chunk* chunk) {
    Parser parser;
    parser.compilingChunk = chunk;
    parser.hadError = false;
    parser.panicMode = false;

    Scanner scanner(source);
    parser.scanner = &scanner;

    // Initialize parser by advancing to first token
    parser.advance();

    // Parse expression
    parser.expression();
    parser.consume(TokenType::TOKEN_EOF, "Expect end of expression.");

    // Emit return
    parser.emitReturn();

#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
        disassembleChunk(*chunk, "code");
    }
#endif

    return !parser.hadError;
}
