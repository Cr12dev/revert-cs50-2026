cmake_minimum_required(VERSION 3.10)

project(RevertCS50 VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/main.c 
    src/mi_lib.c)
