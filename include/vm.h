#ifndef LOX_VM_HPP
#define LOX_VM_HPP

#include "common.h"
#include "chunk.h"
#include <string_view>

class VM {
    public:
        VM();
        ~VM();

        // Initilize and free VM resources
        void initVM();
        void freeVM();

        InterpretResult interpret(std::string_view source);

    private:
        Chunk* chunk;
        uint8_t* ip; // instruction pointer
        std::vector<Value> stack;

        // Stack operations
        void push(Value value);
        Value pop();

        //Execution
        InterpretResult run();

        // Debug
        static void resetStack();
};

#endif
