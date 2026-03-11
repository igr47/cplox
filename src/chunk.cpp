#include "chunk.h"

Chunk::Chunk() {
    // Initialize
}

void Chunk::write(uint8_t byte, int line) {
    code.push_back(byte);
    lines.push_back(line);
}

int Chunk::addConstant(Value value)  {
    constants.write(value);
    return constants.values.size() - 1;
}

void Chunk::free() {
    code.clear();
    lines.clear();
    constants.free();
}

void Chunk::writeConstant(Value value) {
    // This is a convenience method that adds a constant and emits an OP_CONSTANT
    uint8_t constant = static_cast<uint8_t>(addConstant(value));
    write(static_cast<uint8_t>(OpCode::OP_CONSTANT), lines.empty() ? 0 : lines.back());
    write(constant, lines.empty() ? 0 : lines.back());
}

/*int Chunk::addConstant(Value value) {
    constants.write(value);
    return constants.values.size() - 1;
}*/

void ValueArray::write(Value value) {
    values.push_back(value);
}

void ValueArray::free() {
    values.clear();
}

void printValue(Value value) {
    printf("%g", value);
}

// Debug functions
void disassembleChunk(const Chunk& chunk, const char* name) {
    printf("=== %s ===", name);

    for (size_t offset = 0; offset < chunk.code.size(); ) {
        offset = disassembleInstruction(chunk, static_cast<int>(offset));
    }
}

static int constantInstruction(const char* name, const Chunk& chunk, int offset) {
    uint8_t constant = chunk.code[offset + 1];
    printf("%-16s %4d '", name, constant);
    printValue(chunk.constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

int disassembleInstruction(const Chunk& chunk, int offset) {
    printf("%04d", offset);

    if (offset > 0 && chunk.lines[offset] == chunk.lines[offset - 1]) {
        printf("  | ");
    } else {
        printf("%4d", chunk.lines[offset]);
    }

    uint8_t instruction = chunk.code[offset];
    switch (static_cast<OpCode>(instruction)) {
        case OpCode::OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", chunk, offset);
        case OpCode::OP_NEGATE:
            return simpleInstruction("OP_NEGATE", offset);
        case OpCode::OP_ADD:
            return simpleInstruction("OP_ADD", offset);
        case OpCode::OP_SUBTRACT:
            return simpleInstruction("OP_SUBTRACT", offset);
        case OpCode::OP_MULTIPLY:
            return simpleInstruction("OP_MULTIPLY", offset);
        case OpCode::OP_DIVIDE:
            return simpleInstruction("OP_DIVIDE", offset);
        case OpCode::OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        default:
            printf("Unkown opcode %d\n", instruction);
            return offset + 1;
    }
}
