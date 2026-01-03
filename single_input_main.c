#include <stdio.h>

#include "puzzle_solver.h"

/* BUILD_SOLUTION is defined by CMakeLists.txt */
#ifndef BUILD_SOLUTION
#error "BUILD_SOLUTION not defined!"
#endif
/* VERBOSE_DEBUG_OUTPUT is probably defined by CMakeLists.txt */

int main(int argc, char* argv[])
{
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("Built for solution %d\n", BUILD_SOLUTION);
#endif

    const char* DEFAULT_INPUT_FILE = "test_input.txt";
    const char* input_file = DEFAULT_INPUT_FILE;
    if (argc > 1)
    {
        input_file = argv[1];
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("Using input file: %s\n", input_file);
#endif
    }

    FILE* input = fopen(input_file, "r");
    if (input == NULL) {
        printf("ERROR: Failed to open input file!");
        return -1;
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("File opened ...\n");
#endif

    int rv = solve_puzzle(input);

    fclose(input);

    return rv;
}
