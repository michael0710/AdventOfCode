#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../../puzzle_solver.h"

typedef enum
{
    STATE_READ_START,
    STATE_READ_END,
    STATE_PROCESSING
} StateType;

typedef struct
{
    uint64_t start;
    uint64_t end;
} RangeType;

#if BUILD_SOLUTION == 1
static bool isInvalidId(uint64_t id)
{
    /* Since the young Elf was just doing silly patterns, you can find the
       invalid IDs by looking for any ID which is made only of some sequence of
       digits repeated twice. So, 55 (5 twice), 6464 (64 twice), and 123123
       (123 twice) would all be invalid IDs. */
    
    /* as an invalid ID is made of repetitions, every id that contains an odd
       amount of digits must be valid */
    uint64_t temp = id;
    size_t digitCount = 0;
    while (temp > 0)
    {
        digitCount++;
        temp /= 10;
    }
    if (digitCount & 0x1)
    {
        /* => valid id => return false */
        return false;
    }

    /* now we know the id has an even amount of digits */
    /* we also know that an invalid id is made of a sequence that is repeated
       twice. Hence we split the id in half */
    uint64_t moduland = 1;
    for (size_t i=0; i<digitCount/2; i++, moduland *= 10);
    uint64_t halfId = id % moduland;

    /* as we now have the half id, we can state that the given id is invalid,
       if the given id matches the "doubled" half id */
//    printf("Testing id %llu: halfId = %llu, moduland = %llu\n", id, halfId, moduland);
    return (id == (halfId * moduland + halfId));
}
#elif BUILD_SOLUTION == 2
static bool containsInvalidSequence(uint64_t id, size_t sequenceLength)
{
    uint64_t moduland = 1;
    for (size_t i=0; i<sequenceLength; i++, moduland *= 10);

    uint64_t sequence = id % moduland;
    if (sequence == 0)
    {
        return false;
    }

    uint64_t temp = 0;
    for (size_t i=0; temp < id; i++)
    {
        temp *= moduland;
        temp += sequence;
    }
    if (temp == id)
    {
        return true;
    }

    return false;
}

static bool isInvalidIdPart2(uint64_t id)
{
    uint64_t temp = id;
    size_t digitCount = 0;
    while (temp > 0)
    {
        digitCount++;
        temp /= 10;
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("    Testing id %llu: digitCount = %zu ...\n", id, digitCount);
#endif

    const size_t TEST_LENGTH[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    for (size_t i=0; i<(sizeof(TEST_LENGTH)/sizeof(size_t)); i++)
    {
        if (   ((digitCount % TEST_LENGTH[i]) == 0)
            && (digitCount > TEST_LENGTH[i]))
        {
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("    Checking id %llu for sequence length %zu ...\n", id, TEST_LENGTH[i]);
#endif
            bool rv = containsInvalidSequence(id, TEST_LENGTH[i]);
            
            if (rv == true)
            {
                return true;
            }
        }
    }

    return false;
}
#endif

int solve_puzzle(FILE* input)
{
    RangeType rangeUnderTest;
    StateType activeState = STATE_READ_START;
    uint64_t sumOfInvalidIds = 0;
#define TEMP_BUF_SIZE 32
    char temp[TEMP_BUF_SIZE];
    bool finished = false;
    while (!finished)
    {
        switch (activeState)
        {
            case STATE_READ_START:
                {
                    printf("Reading start id ...\n");
                    for (size_t i=0; i<TEMP_BUF_SIZE; i++)
                    {
                        char c = fgetc(input);
                        if (c == '-')
                        {
                            temp[i] = '\0';
                            char* endptr;
                            rangeUnderTest.start = strtoull(temp, &endptr, 10);
                            if (endptr == temp)
                            {
                                /* failed to parse number */
                                printf("ERROR: read \"%s\"", temp);
                                return -1;
                            }
                            activeState = STATE_READ_END;
                            break;
                        }
                        else if (c == EOF)
                        {
                            finished = true;
                            break;
                        }
                        temp[i] = c;
                    }
                    if (   (activeState == STATE_READ_END)
                        || (finished == true))
                    {
                        break;
                    }
                    else
                    {
                        /* fail if we are in this state for too long */
                        printf("ERROR: read too many characters \"%32s\"", temp);
                        return -1;
                    }
                }
            case STATE_READ_END:
                {
                    printf("Reading end id ...\n");
                    for (size_t i=0; i<TEMP_BUF_SIZE; i++)
                    {
                        char c = fgetc(input);
                        if (   (c == ',')
                            || (c == '\n'))
                        {
                            temp[i] = '\0';
                            char* endptr;
                            rangeUnderTest.end = strtoull(temp, &endptr, 10);
                            if (endptr == temp)
                            {
                                /* failed to parse number */
                                printf("ERROR: read \"%s\"", temp);
                                return -1;
                            }
                            activeState = STATE_PROCESSING;
                            break;
                        }
                        else if (c == EOF)
                        {
                            finished = true;
                            break;
                        }
                        temp[i] = c;
                    }
                    if (   (activeState == STATE_PROCESSING)
                        || (finished == true))
                    {
                        break;
                    }
                    else
                    {
                        /* fail if we are in this state for too long */
                        printf("ERROR: read too many characters \"%32s\"", temp);
                        return -1;
                    }
                }
            case STATE_PROCESSING:
                {
                    printf("Processing range %llu - %llu ...\n", rangeUnderTest.start, rangeUnderTest.end);
                    for (uint64_t id = rangeUnderTest.start; id <= rangeUnderTest.end; id++)
                    {
#if BUILD_SOLUTION == 1
                        if (isInvalidId(id))
#elif BUILD_SOLUTION == 2
                        if (isInvalidIdPart2(id))
#endif
                        {
                            printf("  found invalid id: %llu\n", id);
                            sumOfInvalidIds += id;
                        }
                    }
                    activeState = STATE_READ_START;
                    break;
                }
            default:
                return -1;
        }
    }

    printf("Sum of invalid IDs is %llu\n", sumOfInvalidIds);

    return 0;
}