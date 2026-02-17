#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

#include "vm.hpp"

class Lox {
    public:
        Lox() : vm() {
            vm.initVM();
        }

        ~Lox() {
            vm.freeVM();
        }

        void repl() {
            std::string line;
            for (;;) {
                std::cout << "> ";

                if (!std::getline(std::cin, line)){
                    std::cout << "\n";
                    break;
                }

                vm.interpret(line);
            }
        }

        void runFile(const std::string& path) {
            std::string source = readFile(path);
            InterpretResult result = vm.interpret(source);

            if (result == InterpretResult::COMPILE_ERROR) exit(65);
            if (result == InterpretEsult::RUNTIME_ERROR) exit(70);
        }
    private:
        VM vm;

        std::string readFile(const std::string& path){
            std::ifstream file(path, std::ios::binary);
            if (!file.is_open()){
                std::cerr << "Could not open file '" << path << ".\n";
                exit(74);
            }

            // Get file size
            file.seekg(0, std::ios::end);
            size_t size = file.tellg();
            file.seekg(0, std::ios::beg);

            // Read file
            std::string buffer(size, '\0');
            file.read(&buffer[0], size);

            if (file.gcount() < static_cast<std::streamsize>(size)) {
                std::cerr << "Could not read file '" << path << "'.\n";
                exit(74);
            }

            return buffer;
        }
}

int main(int argc, const char* argv[]){
    if (argc > 2) {
        std::cerr << "Usage: clox [path]\n";
        exit(64);
    }

    Lox lox;

    if (argc == 1) {
        lox.repl();
    } else if (argc == 2) {
        lox.runFile(argv[1]);
    }
     return 0;
}
