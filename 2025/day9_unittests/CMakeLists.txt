# store the root folder name in the variable rootDir to simplify copying of
# this CMakeLists file
set(aocDay "day9")

# Create one executable for each subtask
file(GLOB_RECURSE srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

add_executable(2025_${aocDay}_p2_unittests "${srcs}")
target_include_directories(2025_${aocDay}_p2_unittests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(2025_${aocDay}_p2_unittests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../day9)
target_compile_definitions(2025_${aocDay}_p2_unittests PRIVATE -DBUILD_SOLUTION=2)
target_compile_definitions(2025_${aocDay}_p2_unittests PRIVATE -DVERBOSE_DEBUG_OUTPUT=1)

# check part 1 with each test_input for return value 0
add_test(NAME 2025_${aocDay}_p2_unittests_rv0
        COMMAND 2025_${aocDay}_p2_unittests)
