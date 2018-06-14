cmake_minimum_required(VERSION 3.8)
project(Ps2_Zad3)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c)
add_executable(Ps2_Zad3 ${SOURCE_FILES})

MATH(EXPR stack_size "512 * 1024 * 1024") # 512 Mb
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--stack,${stack_size}")