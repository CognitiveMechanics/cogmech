cmake_minimum_required(VERSION 3.25)
project(cogmech C)

set(CMAKE_C_STANDARD 11)

include_directories(src)
include_directories(tests/unit)

add_executable(cogmech
        src/main.c)
