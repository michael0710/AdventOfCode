#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "../../puzzle_solver.h"

typedef struct
{
    double x;
    double y;
    double z;
    size_t connectionGroup;
} JunctionBoxType;

typedef struct day8
{
    size_t a;
    size_t b;
} JunctionBoxConnectionType;


int compareConnectionGroupOccs(const void* a, const void* b)
{
    return (-1)*(int)((int64_t)(*(size_t*)a) - (int64_t)(*(size_t*)b));
}

/* NO_OF_SHORTEST_CONNS is defined by the CMakeLists.txt file */

double straight_line_distance(const JunctionBoxType* boxA, const JunctionBoxType* boxB)
{
    return sqrt(  (boxA->x - boxB->x)*(boxA->x - boxB->x)
                + (boxA->y - boxB->y)*(boxA->y - boxB->y)
                + (boxA->z - boxB->z)*(boxA->z - boxB->z) );
}

int solve_puzzle(FILE* input)
{
#define DIMENSION 3
#define MAX_INPUT_LINE_LENGTH   32
#define REALLOC_STEP_SIZE        2
    int funcRv = 0;
    char tempBuf[MAX_INPUT_LINE_LENGTH];
    size_t junctionBoxStorageLength = 1;
    JunctionBoxType* junctionBoxStorage = (JunctionBoxType*)malloc(junctionBoxStorageLength*sizeof(JunctionBoxType));
    size_t junctionBoxIterator = 0;
#if BUILD_SOLUTION == 1
    size_t junctionBoxConnectionStorageLength = 1;
    JunctionBoxConnectionType* junctionBoxConnectionStorage = (JunctionBoxConnectionType*)malloc(junctionBoxConnectionStorageLength*sizeof(JunctionBoxConnectionType));
    size_t junctionBoxConnectionIterator = 0;
#endif
    
#if BUILD_SOLUTION == 2
    if (junctionBoxStorage == NULL)
#elif BUILD_SOLUTION == 1
    if ((junctionBoxStorage == NULL) || (junctionBoxConnectionStorage == NULL))
#endif
    {
        funcRv = -1;
        printf("ERROR: malloc() failed!\n");
    }

    while (funcRv == 0)
    {
        char* rv = fgets(tempBuf, MAX_INPUT_LINE_LENGTH, input);
        if (rv == NULL)
        {
            break;
        }
        if (strnlen(tempBuf, MAX_INPUT_LINE_LENGTH) >= (MAX_INPUT_LINE_LENGTH-1))
        {
            funcRv = -1;
            printf("ERROR: Input line too long!\n");
            break;
        }
        if (junctionBoxIterator >= junctionBoxStorageLength)
        {
            junctionBoxStorageLength += REALLOC_STEP_SIZE;
            JunctionBoxType* newJunctionBoxStorage = (JunctionBoxType*)realloc(junctionBoxStorage, junctionBoxStorageLength*sizeof(JunctionBoxType));
            if (newJunctionBoxStorage == NULL)
            {
                funcRv = -1;
                printf("ERROR: realloc() failed!\n");
                break;
            }
            junctionBoxStorage = newJunctionBoxStorage;
        }

        char* startptr = tempBuf;
        for (size_t i=0; i<DIMENSION; i++)
        {
            char* endptr;
            (&(junctionBoxStorage[junctionBoxIterator].x))[i] = (double)strtoul(startptr, &endptr, 10);
            if (endptr == startptr)
            {
                funcRv = -1;
                printf("ERROR: strtoul() failed to parse coordinate %zu for point %zu!\n", i, junctionBoxIterator);
                break;
            }
            junctionBoxStorage[junctionBoxIterator].connectionGroup = 0;
            startptr = endptr + 1; /* skip , delimiter */
        }
        junctionBoxIterator++;
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("Parsed %zu junction boxes from input file\n", junctionBoxIterator);
#endif

    size_t pointAIndex;
    size_t pointBIndex;

    if (funcRv == 0)
    {
        size_t activeConnectionGroup = 1;
#if BUILD_SOLUTION == 1
        for (size_t k=0; k<NO_OF_SHORTEST_CONNS; k++)
#elif BUILD_SOLUTION == 2
        bool shutTheFuckUp = false;
        size_t k=0;
        do
#endif
        {
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("Calculating %zust/nd/rd/th shortest distance: ", k);
#endif
#if BUILD_SOLUTION == 2
            k++;
#endif
            double shortestDistance = -1.0;
            pointAIndex = 0;
            pointBIndex = 0;
            /* find shortest distance */
            for (size_t i=0; i<junctionBoxIterator; i++)
            {
                for (size_t j=i+1; j<junctionBoxIterator; j++)
                {
                    double distance = straight_line_distance(&junctionBoxStorage[i], &junctionBoxStorage[j]);
#if BUILD_SOLUTION == 1
                    bool connectionAlreadyMade = false;
                    for (size_t m=0; m<junctionBoxConnectionIterator; m++)
                    {
                        if (   (   (junctionBoxConnectionStorage[m].a == i)
                                && (junctionBoxConnectionStorage[m].b == j))
                            || (   (junctionBoxConnectionStorage[m].a == j)
                                && (junctionBoxConnectionStorage[m].b == i)))
                        {
                            connectionAlreadyMade = true;
                            break;
                        }
                    }
#endif

                    if (   (   (shortestDistance < 0.0)
                            || (distance < shortestDistance))
#if BUILD_SOLUTION == 1
                        && (connectionAlreadyMade == false))
#elif BUILD_SOLUTION == 2
                        && (   (junctionBoxStorage[i].connectionGroup != junctionBoxStorage[j].connectionGroup)
                            || (junctionBoxStorage[i].connectionGroup == 0)))
#endif
                    {
                        shortestDistance = distance;
                        pointAIndex = i;
                        pointBIndex = j;
                    }
                }
            }
#if BUILD_SOLUTION == 1
            if (junctionBoxConnectionIterator >= junctionBoxConnectionStorageLength)
            {
                junctionBoxConnectionStorageLength += REALLOC_STEP_SIZE;
                JunctionBoxConnectionType* newJunctionBoxConnectionStorage = (JunctionBoxConnectionType*)realloc(junctionBoxConnectionStorage, junctionBoxConnectionStorageLength*sizeof(JunctionBoxConnectionType));
                if (newJunctionBoxConnectionStorage == NULL)
                {
                    funcRv = -1;
                    printf("ERROR: realloc() failed!\n");
                    break;
                }
                junctionBoxConnectionStorage = newJunctionBoxConnectionStorage;
            }

            junctionBoxConnectionStorage[junctionBoxConnectionIterator].a = pointAIndex;
            junctionBoxConnectionStorage[junctionBoxConnectionIterator].b = pointBIndex;
            junctionBoxConnectionIterator++;
#endif
            /* make connection */
            size_t group2assign = (junctionBoxStorage[pointAIndex].connectionGroup > junctionBoxStorage[pointBIndex].connectionGroup) ?
                                   junctionBoxStorage[pointAIndex].connectionGroup : junctionBoxStorage[pointBIndex].connectionGroup;
            size_t group2replug = (junctionBoxStorage[pointAIndex].connectionGroup < junctionBoxStorage[pointBIndex].connectionGroup) ?
                                   junctionBoxStorage[pointAIndex].connectionGroup : junctionBoxStorage[pointBIndex].connectionGroup;
            if (group2assign == 0)
            {
                group2assign = activeConnectionGroup;
                activeConnectionGroup++;
            }
            junctionBoxStorage[pointAIndex].connectionGroup = group2assign;
            junctionBoxStorage[pointBIndex].connectionGroup = group2assign;
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("%f between box %zu and %zu, assigned to %zu\n", shortestDistance, pointAIndex, pointBIndex, group2assign);
#endif
            if (group2replug != 0)
            {
                for (size_t i=0; i<junctionBoxIterator; i++)
                {
                    if (junctionBoxStorage[i].connectionGroup == group2replug)
                    {
#ifdef VERBOSE_DEBUG_OUTPUT
                        printf("    replugged junction box %zu to group %zu\n", i, group2assign);
#endif
                        junctionBoxStorage[i].connectionGroup = group2assign;
                    }
                }
            }
#if BUILD_SOLUTION == 1
        }
#elif BUILD_SOLUTION == 2
            /* check if all junction boxes are connected */
            shutTheFuckUp = true;
            size_t numberToShutTheFuckUp = junctionBoxStorage[0].connectionGroup;
            for (size_t i=0; i<junctionBoxIterator; i++)
            {
                if (junctionBoxStorage[i].connectionGroup != numberToShutTheFuckUp)
                {
                    shutTheFuckUp = false;
                    break;
                }
            }
        }
        while (shutTheFuckUp == false);
#endif

        /* assign each unconnected junction box to its own group */
        for (size_t i=0; i<junctionBoxIterator; i++)
        {
            if (junctionBoxStorage[i].connectionGroup == 0)
            {
                junctionBoxStorage[i].connectionGroup = activeConnectionGroup;
                activeConnectionGroup++;
            }
        }
    }

    /* find the largest circuits */
    size_t* connectionGroupOccurences = (size_t*)malloc(junctionBoxIterator*sizeof(size_t));
    if (connectionGroupOccurences == NULL)
    {
        funcRv = -1;
        printf("ERROR: malloc() failed!\n");
    }

    if (funcRv == 0)
    {
        memset(connectionGroupOccurences, 0, junctionBoxIterator*sizeof(size_t));
        for (size_t i=0; i<junctionBoxIterator; i++)
        {
            connectionGroupOccurences[junctionBoxStorage[i].connectionGroup]++;
        }

#ifdef VERBOSE_DEBUG_OUTPUT
        printf("connectionGroupOccurences before sorting:");
        for (size_t i=0; i<junctionBoxIterator; i++)
        {
            printf(" %zu", connectionGroupOccurences[i]);
        }
        printf("\n");
#endif

        qsort(connectionGroupOccurences, junctionBoxIterator, sizeof(size_t), compareConnectionGroupOccs);

#ifdef VERBOSE_DEBUG_OUTPUT
        printf("connectionGroupOccurences after sorting: ");
        for (size_t i=0; i<junctionBoxIterator; i++)
        {
            printf(" %zu", connectionGroupOccurences[i]);
        }
        printf("\n");
#endif
    }
 
#if BUILD_SOLUTION == 1
    free(junctionBoxConnectionStorage);
#endif

    uint64_t largestCircuitsMultiplied = 1;
    for (size_t i=0; i<3; i++)
    {
        largestCircuitsMultiplied *= connectionGroupOccurences[i];
    }

    free(connectionGroupOccurences);

#if BUILD_SOLUTION == 1
    printf("The largest circuits multiplied give %llu\n", largestCircuitsMultiplied);
#elif BUILD_SOLUTION == 2
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("The last two connected junction boxes are %zu and %zu\n", pointAIndex, pointBIndex);
    printf("The last two x coordinates are %zu and %zu\n", (size_t)junctionBoxStorage[pointAIndex].x, (size_t)junctionBoxStorage[pointBIndex].x);
#endif
    printf("Multiplication of the last two x coordinates gives %zu\n", (size_t)(junctionBoxStorage[pointAIndex].x * junctionBoxStorage[pointBIndex].x));
#endif
    free(junctionBoxStorage);
    return 0;
}
