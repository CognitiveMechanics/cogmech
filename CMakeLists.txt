cmake_minimum_required(VERSION 3.25)
project(cogmech C)

set(CMAKE_C_STANDARD 11)

include_directories(src)
include_directories(tests/unit)

add_executable(cogmech
        src/util.h
        src/file.c
        src/stringview.c
        src/tokenizer.c
        src/parser.c
        src/interpreter.c
        src/main.c src/parser.c)

add_executable(test
        src/util.h
        src/file.c
        src/stringview.c
        src/tokenizer.c
        src/interpreter.c
        tests/unit/test.c src/parser.c)

target_compile_options(cogmech PUBLIC -Wall -Wextra -std=c++17 -pedantic)
target_compile_options(test PUBLIC -Wall -Wextra -std=c++17 -pedantic)