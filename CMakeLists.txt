cmake_minimum_required(VERSION 3.25)
project(cogmech C)

set(CMAKE_C_STANDARD 11)

include_directories(src)
include_directories(tests/unit)

add_executable(cogmech
        src/file.c
        src/util.c
        src/stringview.c
        src/tokenizer.c
        src/parser.c
        src/interpreter.c
        src/main.c)

add_executable(test
        src/file.c
        src/util.c
        src/stringview.c
        src/tokenizer.c
        src/interpreter.c
        src/parser.c
        tests/unit/unit.c
        tests/unit/test_file.c
        tests/unit/test_func.c
        tests/unit/test_tokenizer.c
        tests/unit/test_parser.c
        tests/unit/test_interpreter.c
        tests/func/func.c
        tests/main.c)

add_executable(func
        src/file.c
        src/util.c
        tests/func/func.c
        tests/func/main.c)

target_compile_options(cogmech PUBLIC -Wall -Wextra -std=c++17 -pedantic)
target_compile_options(test PUBLIC -Wall -Wextra -std=c++17 -pedantic)
target_compile_options(func PUBLIC -Wall -Wextra -std=c++17 -pedantic)