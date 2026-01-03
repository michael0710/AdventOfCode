#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../../puzzle_solver.h"

typedef struct
{
    unsigned long long start_id;
    unsigned long long end_id;
} IdRangeType;

#if BUILD_SOLUTION == 1
static bool id_in_range(size_t id, IdRangeType range)
{
    return (id >= range.start_id) && (id <= range.end_id);
}
#elif BUILD_SOLUTION == 2
static void swap_ids(IdRangeType *restrict range1, IdRangeType *restrict range2)
{
    IdRangeType temp = *range1;
    *range1 = *range2;
    *range2 = temp;
}

static void sort_id_ranges(IdRangeType* ranges, size_t rangeCount)
{
    /* simple bubble sort */
    for (size_t i=0; i<rangeCount-1; i++)
    {
        for (size_t j=0; j<rangeCount-i-1; j++)
        {
            if (ranges[j].start_id > ranges[j+1].start_id)
            {
                swap_ids(&ranges[j], &ranges[j+1]);
            }
        }
    }
}

static size_t merge_id_ranges(IdRangeType* ranges, size_t rangeCount)
{
    size_t mergedCount = 0;
    for (size_t i=0; i<rangeCount-1; i++)
    {
        if (ranges[i].end_id >= ranges[i+1].start_id)
        {
            /* merge ranges */
            if (ranges[i+1].end_id > ranges[i].end_id)
            {
                ranges[i].end_id = ranges[i+1].end_id;
            }
            /* shift remaining ranges down */
            for (size_t j=i+1; j<rangeCount-1; j++)
            {
                ranges[j] = ranges[j+1];
            }
            rangeCount--;
            mergedCount++;
            i--; /* recheck this index */
        }
    }
    return mergedCount;
}

#endif

int solve_puzzle(FILE* input)
{
#define MAX_INPUT_CHARS 64
#define ID_RANGE_BLOCK_SIZE 64
    int funcRv = 0;
    size_t freshIngredients = 0;
    char tempBuf[MAX_INPUT_CHARS];
    size_t idRangeLength = ID_RANGE_BLOCK_SIZE;
    size_t idRangeIterator = 0;
    IdRangeType* idRanges = malloc(sizeof(IdRangeType) * idRangeLength);
    /* parse id ranges */
    while (funcRv == 0)
    {
        char* rv = fgets(tempBuf, MAX_INPUT_CHARS, input);
        if (rv == NULL)
        {
            break;
        }
        if (strnlen(tempBuf, MAX_INPUT_CHARS) >= (MAX_INPUT_CHARS-1))
        {
            funcRv = -1;
            printf("ERROR: Input line too long!\n");
            break;
        }
        if (tempBuf[0] == '\n')
        {
            /* blank line indicates end of ranges */
            break;
        }
        if (idRangeIterator >= idRangeLength)
        {
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("Reallocating memory for id ranges...\n");
#endif
            idRangeLength += ID_RANGE_BLOCK_SIZE;
            IdRangeType* newRanges = realloc(idRanges, sizeof(IdRangeType) * idRangeLength);
            if (newRanges == NULL)
            {
                funcRv = -1;
                printf("ERROR: Failed to realloc memory for id ranges!\n");
                break;
            }
            idRanges = newRanges;
        }
        char* endptrStart = NULL;
        idRanges[idRangeIterator].start_id = strtoull(tempBuf, &endptrStart, 10);
        if ((endptrStart == NULL) || (*endptrStart != '-'))
        {
            funcRv = -1;
            printf("ERROR: Failed to parse start id from line: %s\n", tempBuf);
            break;
        }
        char* endptrEnd = NULL;
        idRanges[idRangeIterator].end_id = strtoull(endptrStart+1, &endptrEnd, 10);
        if (endptrEnd == NULL)
        {
            funcRv = -1;
            printf("ERROR: Failed to parse end id from line: %s\n", tempBuf);
            break;
        }
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("Found new id range %llu - %llu\n", idRanges[idRangeIterator].start_id, idRanges[idRangeIterator].end_id);
#endif
        idRangeIterator++;
    }
#if BUILD_SOLUTION == 1
    /* parse ids */
    while (funcRv == 0)
    {
        char* rv = fgets(tempBuf, MAX_INPUT_CHARS, input);
        if (rv == NULL)
        {
            break;
        }
        if (strnlen(tempBuf, MAX_INPUT_CHARS) >= (MAX_INPUT_CHARS-1))
        {
            funcRv = -1;
            printf("ERROR: Input line too long!\n");
            break;
        }
        if (tempBuf[0] == '\n')
        {
            /* blank line indicates end of ids */
            break;
        }
        
        char* endptrStart = NULL;
        unsigned long long idUnderTest = strtoull(tempBuf, &endptrStart, 10);
        if (endptrStart == NULL)
        {
            funcRv = -1;
            printf("ERROR: Failed to parse id from line: %s\n", tempBuf);
            break;
        }

        for (size_t i=0; i<idRangeIterator; i++)
        {
            if (id_in_range(idUnderTest, idRanges[i]))
            {
                freshIngredients++;
                break;
            }
        }
    }
#elif BUILD_SOLUTION == 2
    bool finished = false;
    while (finished == false)
    {
        sort_id_ranges(idRanges, idRangeIterator);
        size_t mergedRanges = merge_id_ranges(idRanges, idRangeIterator);
        idRangeIterator -= mergedRanges;
        finished = (mergedRanges == 0);
    }
    for (size_t i=0; i<idRangeIterator; i++)
    {
        freshIngredients += (idRanges[i].end_id - idRanges[i].start_id + 1);
    }
#endif

    free(idRanges);

    printf("Number of fresh ingredients: %zu\n", freshIngredients);
    return funcRv;
}
