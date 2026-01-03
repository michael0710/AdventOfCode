cmake_minimum_required(VERSION 3.20)
project(AdventOfCode)

set(AOC_SINGLE_INPUT_MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/single_input_main.c")
set(AOC_GLOBAL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

include(CTest)
enable_testing()
set(CTEST_TEST_TIMEOUT 10) # does not work right now...
add_subdirectory(2025)
