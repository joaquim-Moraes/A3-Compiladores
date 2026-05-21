cmake_minimum_required(VERSION 3.20)

project(compilador LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(compilador
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/evaluator.cpp
)

target_include_directories(compilador PRIVATE src)

target_compile_options(compilador PRIVATE
    -Wall
    -Wextra
    -pedantic
)
