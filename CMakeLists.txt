cmake_minimum_required(VERSION 3.10)
project(lox)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(clox
    src/main.cpp
)
