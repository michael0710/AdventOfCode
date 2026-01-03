# store the root folder name in the variable rootDir to simplify copying of
# this CMakeLists file
set(aocDay "day7")

# Create one executable for each subtask
file(GLOB_RECURSE srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
list(APPEND srcs "${AOC_SINGLE_INPUT_MAIN_SRC}")

add_executable(2025_${aocDay}_p1 "${srcs}")
target_include_directories(2025_${aocDay}_p1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(2025_${aocDay}_p1 PRIVATE ${AOC_GLOBAL_INCLUDE_DIR})
target_compile_definitions(2025_${aocDay}_p1 PRIVATE -DBUILD_SOLUTION=1)
#target_compile_definitions(2025_${aocDay}_p1 PRIVATE -DVERBOSE_DEBUG_OUTPUT=1)

add_executable(2025_${aocDay}_p2 "${srcs}")
target_include_directories(2025_${aocDay}_p2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(2025_${aocDay}_p2 PRIVATE ${AOC_GLOBAL_INCLUDE_DIR})
target_compile_definitions(2025_${aocDay}_p2 PRIVATE -DBUILD_SOLUTION=2)
#target_compile_definitions(2025_${aocDay}_p2 PRIVATE -DVERBOSE_DEBUG_OUTPUT=1)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/input.txt"
               "${CMAKE_CURRENT_BINARY_DIR}/input.txt"
               COPYONLY)
file(GLOB_RECURSE test_inputs "${CMAKE_CURRENT_SOURCE_DIR}/test_input*.txt")
foreach(test_file ${test_inputs})
    configure_file("${test_file}"
                   "${CMAKE_CURRENT_BINARY_DIR}"
                   COPYONLY)
        
    get_filename_component(test_filename "${test_file}" NAME_WE)

    # check part 1 with each test_input for return value 0
    add_test(NAME 2025_${aocDay}_p1_${test_filename}_rv0
            COMMAND 2025_${aocDay}_p1 ${test_file})

    # check part 2 with each test_input for return value 0
    add_test(NAME 2025_${aocDay}_p2_${test_filename}_rv0
            COMMAND 2025_${aocDay}_p2 ${test_file})

endforeach()

# check part 1 with each test_input1 for matching output
add_test(NAME 2025_${aocDay}_p1_test_input1
         COMMAND 2025_${aocDay}_p1 test_input1.txt)

set_property(TEST 2025_${aocDay}_p1_test_input1
             PROPERTY PASS_REGULAR_EXPRESSION "[.]*The beams will be split 1 times\n")

# check part 1 with each test_input2 for matching output
add_test(NAME 2025_${aocDay}_p1_test_input2
         COMMAND 2025_${aocDay}_p1 test_input2.txt)

set_property(TEST 2025_${aocDay}_p1_test_input2
             PROPERTY PASS_REGULAR_EXPRESSION "[.]*The beams will be split 6 times\n")

# check part 1 with each test_input3 for matching output
add_test(NAME 2025_${aocDay}_p1_test_input3
         COMMAND 2025_${aocDay}_p1 test_input3.txt)

set_property(TEST 2025_${aocDay}_p1_test_input3
             PROPERTY PASS_REGULAR_EXPRESSION "[.]*The beams will be split 5 times\n")

# check part 1 with each test_input4 for matching output
add_test(NAME 2025_${aocDay}_p1_test_input4
         COMMAND 2025_${aocDay}_p1 test_input4.txt)

set_property(TEST 2025_${aocDay}_p1_test_input4
             PROPERTY PASS_REGULAR_EXPRESSION "[.]*The beams will be split 7 times\n")

# check part 1 with each test_input5 for matching output
add_test(NAME 2025_${aocDay}_p1_test_input5
         COMMAND 2025_${aocDay}_p1 test_input5.txt)

set_property(TEST 2025_${aocDay}_p1_test_input5
             PROPERTY PASS_REGULAR_EXPRESSION "[.]*The beams will be split 21 times\n")

# check part 2 with each test_input1 for matching output
add_test(NAME 2025_${aocDay}_p2_test_input1
         COMMAND 2025_${aocDay}_p2 test_input1.txt)

set_property(TEST 2025_${aocDay}_p2_test_input1
             PROPERTY PASS_REGULAR_EXPRESSION "[.]*There exist 2 different timelines\n")

# check part 2 with each test_input2 for matching output
add_test(NAME 2025_${aocDay}_p2_test_input2
         COMMAND 2025_${aocDay}_p2 test_input2.txt)

set_property(TEST 2025_${aocDay}_p2_test_input2
             PROPERTY PASS_REGULAR_EXPRESSION "[.]*There exist 8 different timelines\n")

# check part 2 with each test_input3 for matching output
add_test(NAME 2025_${aocDay}_p2_test_input3
         COMMAND 2025_${aocDay}_p2 test_input3.txt)

set_property(TEST 2025_${aocDay}_p2_test_input3
             PROPERTY PASS_REGULAR_EXPRESSION "[.]*There exist 6 different timelines\n")

# check part 2 with each test_input4 for matching output
add_test(NAME 2025_${aocDay}_p2_test_input4
         COMMAND 2025_${aocDay}_p2 test_input4.txt)

set_property(TEST 2025_${aocDay}_p2_test_input4
             PROPERTY PASS_REGULAR_EXPRESSION "[.]*There exist 8 different timelines\n")

# check part 2 with each test_input5 for matching output
add_test(NAME 2025_${aocDay}_p2_test_input5
         COMMAND 2025_${aocDay}_p2 test_input5.txt)

set_property(TEST 2025_${aocDay}_p2_test_input5
             PROPERTY PASS_REGULAR_EXPRESSION "[.]*There exist 40 different timelines\n")

# check part 2 with actual input for return value
add_test(NAME 2025_${aocDay}_p2_input_rv0
         COMMAND 2025_${aocDay}_p2 input.txt)

# check part 2 with actual input for matching output
# note that this test is necessary, as the other test cases work, but are small enough
# to avoid integer overflows if integer datatypes smaller than 64-bit are used.
add_test(NAME 2025_${aocDay}_p2_input
         COMMAND 2025_${aocDay}_p2 input.txt)

set_property(TEST 2025_${aocDay}_p2_input
             PROPERTY PASS_REGULAR_EXPRESSION "[.]*There exist 32451134474991 different timelines\n")
