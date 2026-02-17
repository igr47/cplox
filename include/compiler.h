#ifndef LOX_COMPILER_HPP
#define LOX_COMPILER_HPP

#include <string_view>


class Compiler {
    public:
        explicit Compiler() = default;

        // Compile source code
        bool compile(std::string_view source);

    private:
        // Expansion in later topics
};

#endif
