# store the root folder name in the variable rootDir to simplify copying of
# this CMakeLists file
set(aocDay "day5")

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

    # check part 1 with each test_input for matching output
    add_test(NAME 2025_${aocDay}_p1_${test_filename}
            COMMAND 2025_${aocDay}_p1 ${test_file})

    set_property(TEST 2025_${aocDay}_p1_${test_filename}
                PROPERTY PASS_REGULAR_EXPRESSION "[.]*Number of fresh ingredients: 3\n")

    # check part 2 with each test_input for return value 0
    add_test(NAME 2025_${aocDay}_p2_${test_filename}_rv0
            COMMAND 2025_${aocDay}_p2 ${test_file})

    # check part 2 with each test_input for matching output
    add_test(NAME 2025_${aocDay}_p2_${test_filename}
            COMMAND 2025_${aocDay}_p2 ${test_file})

    set_property(TEST 2025_${aocDay}_p2_${test_filename}
                PROPERTY PASS_REGULAR_EXPRESSION "[.]*Number of fresh ingredients: 14\n")
endforeach()
