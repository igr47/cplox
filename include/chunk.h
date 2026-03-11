#ifndef LOX_CHUNK_H
#define LOX_CHUNK_H

#include <vector>
#include <cstdint>
#include "value.h"

// Opcodes for bytecode instructions
enum class OpCode : uint8_t {
    OP_CONSTANT,
    OP_NEGATE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_RETURN,
};

class Chunk {
    public:
        std::vector<uint8_t> code;
        std::vector<int> lines;
        ValueArray constants;
        Chunk();
        ~Chunk() = default;

        void write(uint8_t byte, int line);
        void writeConstant(Value value);
        int addConstant(Value value);
        void free();
};

// Debug functions
void disassembleChunk(const Chunk& chunk, const char* name);
int disassembleInstruction(const Chunk& chunk, int offset);

#endif
