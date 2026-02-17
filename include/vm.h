#ifndef LOX_VM_HPP
#define LOX_VM_HPP

#include "common.h"
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
        // We'll expand in later chapters
};

#endif
