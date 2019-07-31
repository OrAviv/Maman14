cmake_minimum_required(VERSION 3.14)
project(Maman14 C)

set(CMAKE_C_STANDARD 99)

add_executable(Maman14 assembler.c data.c data.h runner.c hashtable.c hashtable.h)