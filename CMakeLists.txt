cmake_minimum_required(VERSION 3.10)
project(lox)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(clox
    src/main.cpp
    src/compiler.cpp
    src/scanner.cpp
    src/vm.cpp
    src/chunk.cpp
    src/value.cpp
)
