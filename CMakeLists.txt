##########################################################################
# Project settings
##########################################################################
cmake_minimum_required(VERSION 3.1.0)
project(lc3vm VERSION 0.3.0)
set(PROJECT_NAME lc3vm)

set (CMAKE_C_STANDARD 11)
##########################################################################
# Build project
##########################################################################
add_compile_options(
    -Wall
    -Os
)

add_executable(lc3vm
src/obj_loader.c
src/utils.c
src/cpu.c
src/main.c
)

target_include_directories(lc3vm PUBLIC
                          "${PROJECT_SOURCE_DIR}/src"
                          )