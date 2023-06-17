cmake_minimum_required(VERSION 3.0)
project(cogmech C)

set(CMAKE_C_STANDARD 11)

include_directories(src)
include_directories(tests/unit)

add_executable(cogmech
        src/file.c
        src/util.c
        src/stringview.c
        src/context.c
        src/token.c
        src/tokenlist.c
        src/node.c
        src/tokenizer.c
        src/parser.c
        src/interpreter.c
        src/main.c)

add_executable(test
        src/file.c
        src/util.c
        src/stringview.c
        src/context.c
        src/token.c
        src/tokenlist.c
        src/node.c
        src/tokenizer.c
        src/interpreter.c
        src/parser.c
        tests/unit/unit.c
        tests/unit/test_file.c
        tests/unit/test_func.c
        tests/unit/test_token.c
        tests/unit/test_tokenlist.c
        tests/unit/test_tokenizer.c
        tests/unit/test_node.c
        tests/unit/test_parser.c
        tests/unit/test_context.c
        tests/unit/test_interpreter.c
        tests/func/func.c
        tests/main.c)

add_executable(func
        src/file.c
        src/util.c
        tests/func/func.c
        tests/func/main.c)

add_compile_options("$<$<CONFIG:Debug>:-Wall -Wextra -pedantic>")

target_compile_options(cogmech PUBLIC -std=c11)
target_compile_options(test PUBLIC -std=c11)
target_compile_options(func PUBLIC -std=c11)

set(INSTALL_DIR "/usr/local/bin")
install(TARGETS cogmech DESTINATION ${INSTALL_DIR})