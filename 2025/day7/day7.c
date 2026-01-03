#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#include "../../puzzle_solver.h"

int solve_puzzle(FILE* input)
{
#define MAX_INPUT_LINE_LENGTH   256
    char tempInputBuf[MAX_INPUT_LINE_LENGTH];
    uint64_t beamBufPrev[MAX_INPUT_LINE_LENGTH] = {0};
    uint64_t beamBufPost[MAX_INPUT_LINE_LENGTH] = {0};

#if BUILD_SOLUTION == 1
    uint64_t numberOfSplits = 0; 
#endif

#ifdef VERBOSE_DEBUG_OUTPUT
    size_t lineCounter = 0;
#endif
    while (1)
    {
        char* rv = fgets(tempInputBuf, MAX_INPUT_LINE_LENGTH, input);
        if (rv == NULL)
        {
            break;
        }
        if (strnlen(tempInputBuf, MAX_INPUT_LINE_LENGTH) >= (MAX_INPUT_LINE_LENGTH-1))
        {
            printf("ERROR: Input line too long!\n");
            return -1;
        }
        memcpy(beamBufPrev, beamBufPost, MAX_INPUT_LINE_LENGTH*sizeof(uint64_t));
        memset(beamBufPost, (int)0, MAX_INPUT_LINE_LENGTH*sizeof(uint64_t));

        size_t len = strnlen(tempInputBuf, MAX_INPUT_LINE_LENGTH)-1;
        for (size_t i=0; i<len; i++)
        {
            char currentChar = tempInputBuf[i];
            if (tempInputBuf[i] == 'S')
            {
                /* start of a beam */
                beamBufPost[i] = 1;
            }
            else if (beamBufPrev[i] != 0)
            {
                if (currentChar == '^')
                {
                    /* beam is split */
#if BUILD_SOLUTION == 1
                    numberOfSplits++;
                    beamBufPost[i+1] = 1;
                    beamBufPost[i-1] = 1;
#elif BUILD_SOLUTION == 2
                    beamBufPost[i+1] += beamBufPrev[i];
                    beamBufPost[i-1] += beamBufPrev[i];
#endif
                }
                else
                {
                    /* beam is forwarded */
#if BUILD_SOLUTION == 1
                    beamBufPost[i] = 1;
#elif BUILD_SOLUTION == 2
                    beamBufPost[i] += beamBufPrev[i];
#endif
                }
            }
            else if (   (currentChar == '.')
                     || (currentChar == '^'))
            {
                /* ignore empty space or splitters that are not hit by beams */
            }
            else
            {
                printf("ERROR: Unknown character '%c' in input!\n", currentChar);
                return -1;
            }
        }
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("tempInputBuf:");
        for (size_t i=0; i<len; i++)
        {
            printf(" %2c", tempInputBuf[i]);
        }
        printf("\n");
        printf("beamBufPost: ");
        for (size_t i=0; i<len; i++)
        {
            printf(" %2llu", beamBufPost[i]);
        }
        printf("\n");
        lineCounter++;
#endif
    }

#if BUILD_SOLUTION == 1
    printf("The beams will be split %llu times\n", numberOfSplits);
#elif BUILD_SOLUTION == 2
    uint64_t numberOfTimelines = 0;
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("beamBufPost:");
#endif
    for (size_t i=0; i<MAX_INPUT_LINE_LENGTH; i++)
    {
#ifdef VERBOSE_DEBUG_OUTPUT
        printf(" %llu", beamBufPost[i]);
#endif
        numberOfTimelines += beamBufPost[i];
    }
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("\n");
        printf("Processed %zu lines\n", lineCounter);
#endif
    /* discount the initial timeline */
    printf("There exist %llu different timelines\n", numberOfTimelines);
#endif
    return 0;
}
