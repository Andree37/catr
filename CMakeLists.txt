cmake_minimum_required(VERSION 3.27)
project(catr C)

set(CMAKE_C_STANDARD 11)

add_executable(catr
        src/catr.c
        src/modes.h)
