#include "vm.h"
#include "compiler.h"
#include <cstdarg>
#include <cstdio>

VM::VM() : chunk(nullptr), ip(nullptr) {
    stack.reserve(256);
}

VM::~VM() = default;

void VM::initVM() {
    resetStack();
}

void VM::freeVM() {
    // Clean up
}

void VM::resetStack() {
    // Clear stack
}

void VM::push(Value value) {
    stack.push_back(value);
}

Value VM::pop() {
    Value value = stack.back();
    stack.pop_back();
    return value;
}

InterpretResult VM::run() {
#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    do {\
        double b = pop(); \
        double a = pop(); \
        push(a op b); \
    } while (false)

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("         ");
        for (const auto& value : stack) {
            printf("[ ");
            printValue(value);
            printf("]");
        }
        printf("\n");
        disassembleInstruction(*chunk, (int)(ip - cunk->code.data()));
#endif
        uint8_t instruction;
        switch (static_cast<OpCode>(instruction = READ_BYTE())) {
            case OpCode::OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OpCode::OP_NEGATE: {
                push(-pop());
                break;
            }
            case OpCode::OP_ADD: BINARY_OP(+); break;
            case OpCode::OP_SUBTRACT: BINARY_OP(-); break;
            case OpCode::OP_MULTIPLY: BINARY_OP(*); break;
            case OpCode::OP_DIVIDE: {
                double b = pop();
                double a = pop();
                push(a / b);
                break;
            }
            case OpCode::OP_RETURN: {
                // Print result
                printValue(pop());
                printf("\n");
                return InterpretResult::OK;
            }
        }
    }
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult VM::interpret(std::string_view source) {
    Chunk chunk;
    Compiler compiler;

    if (!compiler.compile(source, &chunk)) {
        return InterpretResult::COMPILE_ERROR;
    }

    this->chunk = &chunk;
    this->ip = this->chunk->code.data();

    InterpretResult result = run();

    return result;
}
