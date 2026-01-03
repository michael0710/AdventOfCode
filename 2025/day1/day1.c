#include <stdlib.h>
#include <string.h>

#include "../../puzzle_solver.h"

#if BUILD_SOLUTION == 1
/**
 * Performs a step in the given direction from the start position.
 * 
 * \param startpos The starting position.
 * \param instruction The instruction indicating the direction and distance to move.
 * 
 * \return The new position after performing the step.
 */
static int step(int startpos, int instruction)
{
    int pos = startpos + instruction;
    if (pos < 0) {
        do
        {
            pos += 100;
        } while (pos < 0);
    }
    else if (pos > 99)
    {
        do
        {
            pos -= 100;
        } while (pos > 99);
    }
    return pos;
}
#elif BUILD_SOLUTION == 2
/**
 * Performs a step in the given direction from the start position.
 * 
 * \param startpos The starting position.
 * \param instruction The instruction indicating the direction and distance to move.
 * 
 * \return The new position after performing the step.
 */
static int step_w_zerocrossingcounter(int startpos, int instruction, int* zerocrossingcounter)
{
    int pos = startpos + instruction;
    if (pos < 0) {
        if (   (startpos == 0)
            && (instruction < 0))
        {
            (*zerocrossingcounter)--;
        }
        do
        {
            pos += 100;
            (*zerocrossingcounter)++;
        } while (pos < 0);
    }
    else if (pos > 99)
    {
        do
        {
            pos -= 100;
            (*zerocrossingcounter)++;
        } while (pos > 99);
        if (pos == 0)
        {
            (*zerocrossingcounter)--;
        }
    }
    return pos;
}
#endif

int solve_puzzle(FILE* input)
{
    int pos = 50;
    int zerocounter = 0;

    char line[8];
    size_t len = 0;
    while (1)
    {
        char* rv;
        rv = fgets(line, sizeof(line), input);
        if (rv == NULL)
        {
            break;
        }

        int sign = (line[0] == 'L') ? -1 : 1;

        int distance = strtol(&line[1], NULL, 10);
        int instruction = sign * distance;

#if BUILD_SOLUTION == 1
        int newpos = step(pos, instruction);
#elif BUILD_SOLUTION == 2
        int prevzerocounter = zerocounter;
        int newpos = step_w_zerocrossingcounter(pos, instruction, &zerocounter);
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("Step %s: from %d to %d using instruction %d, caused zerocrossings: %d\n", line, pos, newpos, instruction, zerocounter - prevzerocounter);
#endif
#endif

        if (newpos == 0)
        {
            zerocounter++;
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("  Reached zero!\n");
#endif
        }
        pos = newpos;
    }

    printf("\nZerocounter: %d\n", zerocounter);
    return (feof(input) == 0) ? -1 : 0;
}