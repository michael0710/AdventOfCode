#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "hmath.h"

#include "../../puzzle_solver.h"

typedef MatrixType ShapeType;

#define SHAPE_INPUT_COUNT 6

typedef struct
{
    size_t m;
    size_t n;
    size_t shapes2fit[SHAPE_INPUT_COUNT];
} RegionType;

static void free_rainer(ShapeType* shapes, size_t count)
{
    for (size_t i=0; i<count; i++)
    {
        if (shapes != NULL)
        {
            free(shapes[i].a);
            shapes[i].a = NULL;
        }
    }
}

static void print_shape(const ShapeType* A)
{
#define FLOAT_PRINTS    "f"
    const int PRINT_OFFSET = 8;
    const int NUMBER_WIDTH = 8;
    const int INTER_MATRIX_SPACE = 4;
    const int PRINT_PRECISION = 2;

    printf("%*s/", PRINT_OFFSET, "");
    for (size_t col=0; col<(A->n); col++)
    {
        printf("%*s", NUMBER_WIDTH, "");
    }
    printf("\\\n");

    size_t maxRows = (A->m);

    for (size_t row=0; row<maxRows; row++)
    {
        printf("%*s|", PRINT_OFFSET, "");
        for (size_t col=0; col<(A->n); col++)
        {
            printf("%*.*"FLOAT_PRINTS"", NUMBER_WIDTH, PRINT_PRECISION, (A->a)[row*(A->n)+col]);
        }
        printf("|\n");
    }
    printf("%*s\\", PRINT_OFFSET, "");
    for (size_t col=0; col<(A->n); col++)
    {
        printf("%*s", NUMBER_WIDTH, "");
    }
    printf("/\n");
}

static bool trial_and_rorre(const RegionType* reg, const ShapeType* startArea, const ShapeType* shapes, size_t shapeCount, size_t activeShapeNo)
{
    /* find next shape index */
    size_t activeShapeIndex = 0;
    bool noShapeLeft = true;
    for (size_t i=0; i<shapeCount; i++)
    {
        activeShapeIndex += reg->shapes2fit[i];
        if (activeShapeIndex > activeShapeNo)
        {
            noShapeLeft = false;
            activeShapeIndex = i;
            break;
        }
    }

    if (noShapeLeft)
    {
        return true;
    }

    /* try to fit the shape into the given startArea */
    CoeffType* workShapeContent = (CoeffType*)malloc(sizeof(CoeffType)*startArea->m*startArea->n);
    ShapeType workShape = {
        .m = startArea->m,
        .n = startArea->n,
        .a = workShapeContent,
    };

    CoeffType* activeShapeContent = (CoeffType*)malloc(sizeof(CoeffType)*shapes[activeShapeIndex].m*shapes[activeShapeIndex].n);
    ShapeType activeShape = {
        .m = shapes[activeShapeIndex].m,
        .n = shapes[activeShapeIndex].n,
        .a = activeShapeContent,
    };

    CoeffType* tempShapeContent = (CoeffType*)malloc(sizeof(CoeffType)*shapes[activeShapeIndex].m*shapes[activeShapeIndex].n);
    ShapeType tempShape = {
        .m = shapes[activeShapeIndex].m,
        .n = shapes[activeShapeIndex].n,
        .a = tempShapeContent,
    };

    memcpy(activeShapeContent, shapes[activeShapeIndex].a, sizeof(CoeffType)*shapes[activeShapeIndex].m*shapes[activeShapeIndex].n);

    bool funcRv = false;
    /* try the shape at angles 0, 90, 180 and 270 deg */
    for (size_t i=0; i<4; i++)
    {
        /* turn matrix elements by 90 deg on the second, third and fourth iteration */
        if (i > 0)
        {
            hmath_fliplr(&activeShape, &tempShape);
            hmath_transpose(&tempShape, &activeShape);
        }

        /* try the shape at all possible positions */
        for (size_t k=0; k<=(startArea->m - 3); k++)
        {
            for (size_t q=0; q<=(startArea->n - 3); q++)
            {
                /* reset the workShape content */
                memcpy(workShapeContent, startArea->a, sizeof(CoeffType)*startArea->m*startArea->n);

                hmath_add_elems(&workShape, &activeShape, k, q);

                if (hmath_single_elem_gt(&workShape, 1.5) == false)
                {
                    bool rv = trial_and_rorre(reg, &workShape, shapes, shapeCount, activeShapeNo+1);
                    if (rv == true)
                    {
                        funcRv = true;
                        break;
                    }
                }
            }
            if (funcRv == true)
            {
                break;
            }
        }
        if (funcRv == true)
        {
            break;
        }
    }

    free(tempShapeContent);
    free(activeShapeContent);
    free(workShapeContent);

    return funcRv;
}

static bool is_worth_trying(const RegionType* reg, const ShapeType* shapes, size_t shapeCount)
{
    size_t shapes2add = 0;
    for (size_t i=0; i<shapeCount; i++)
    {
        shapes2add += reg->shapes2fit[i];
    }

    if ((shapes2add*7) > (reg->m*reg->n))
    {
        return false;
    }

    return true;
}

static bool test_region(const RegionType* reg, const ShapeType* shapes, size_t shapeCount)
{
    /* a rotation matrix to rotate the shapes by 90 deg comes in handy */
    CoeffType* workShapeContent = (CoeffType*)malloc(sizeof(CoeffType)*reg->m*reg->n);
    memset(workShapeContent, 0, sizeof(CoeffType)*reg->m*reg->n);
    ShapeType workShape = {
        .m = reg->m,
        .n = reg->n,
        .a = workShapeContent,
    };

    return trial_and_rorre(reg, &workShape, shapes, shapeCount, 0);
}

int solve_puzzle(FILE* input)
{
#define MAX_INPUT_LINE_LENGTH   32
    int funcRv = 0;

    char tempBuf[MAX_INPUT_LINE_LENGTH];

    /* lets simplify the program here by assuming there are exactly SHAPE_INPUT_COUNT shapes
       and each shape fits into a 3x3 shape */
    ShapeType shapeInput[SHAPE_INPUT_COUNT];
    for(size_t i=0; (i<SHAPE_INPUT_COUNT) && (funcRv == 0); i++)
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

        /* be careful if SHAPE_INPUT_COUNT is changed! The statement below must be changed as well! */
        if ((tempBuf[0] > '5') && (tempBuf[0] < '0'))
        {
            funcRv = -1;
            printf("ERROR: Invalid input! Expected digit 0-5, got '0x%02X'!\n", (int)tempBuf[0]);
            break;
        }

        size_t inputIndex = (size_t)(tempBuf[0] - '0');
        shapeInput[inputIndex].m = 3;
        shapeInput[inputIndex].n = 3;
        shapeInput[inputIndex].a = (CoeffType*)malloc(shapeInput[inputIndex].m*
                                                      shapeInput[inputIndex].n*
                                                      sizeof(CoeffType));
        for(size_t i=0; (i<shapeInput[inputIndex].m) && (funcRv == 0); i++)
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
            for(size_t k=0; (k<shapeInput[inputIndex].m) && (funcRv == 0); k++)
            {
                switch (tempBuf[k])
                {
                    case '#':
                    {
                        shapeInput[inputIndex].a[i*shapeInput[inputIndex].m + k] = 1.0;
                        break;
                    }
                    case '.':
                    {
                        shapeInput[inputIndex].a[i*shapeInput[inputIndex].m + k] = 0.0;
                        break;
                    }
                    default:
                        funcRv = -1;
                        printf("ERROR: unexpected input character in shape %zu!\n", inputIndex);
                }
            }
        }

        if (funcRv == 0)
        {
            /* read blank line */
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
        }
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("The input consists of %zu shapes\n"
           "The shapes are:\n", (size_t)SHAPE_INPUT_COUNT);
    for (size_t i=0; i<SHAPE_INPUT_COUNT; i++)
    {
        printf("  %zu:\n", i);
        print_shape(&shapeInput[i]);
        printf("----------------------------------------\n");
    }
#endif

    /* now read the region inputs */
#define REALLOC_STEP_SIZE 2
    RegionType* inputRegions = NULL;
    size_t inputRegionAllocSize = 0; 
    size_t inputRegionCount = 0;
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

        if (inputRegionCount >= inputRegionAllocSize)
        {
            inputRegionAllocSize += REALLOC_STEP_SIZE;
            RegionType* newRegInput = (RegionType*)realloc(inputRegions, inputRegionAllocSize*sizeof(RegionType));
            if (newRegInput == NULL)
            {
                funcRv = -1;
                printf("ERROR: realloc() failed!\n");
                break;
            }
            inputRegions = newRegInput;
        }
        if (funcRv != 0)
        {
            break;
        }

        inputRegions[inputRegionCount].m = 0;
        inputRegions[inputRegionCount].n = 0;

        char* startptr = tempBuf;
        char* endptr = NULL;
        inputRegions[inputRegionCount].m = strtoul(startptr, &endptr, 10);
        if (   (endptr == NULL)
            || (*endptr != 'x'))
        {
            funcRv = -1;
            printf("ERROR: unexpected endptr value (%p) after strtoul for value m!\n", endptr);
            break;
        }

        startptr = endptr + 1;
        endptr == NULL;
        inputRegions[inputRegionCount].n = strtoul(startptr, &endptr, 10);
        if (   (endptr == NULL)
            || (*endptr != ':'))
        {
            funcRv = -1;
            printf("ERROR: unexpected endptr value (%p) after strtoul for value n!\n", endptr);
            break;
        }

        for (size_t i=0; i<SHAPE_INPUT_COUNT; i++)
        {
            startptr = endptr + 1;
            endptr == NULL;
            inputRegions[inputRegionCount].shapes2fit[i] = strtoul(startptr, &endptr, 10);
            if (endptr == NULL)
            {
                funcRv = -1;
                printf("ERROR: unexpected endptr value (%p) after strtoul for shape input %zu!\n", endptr, i);
                break;
            }
        }

        inputRegionCount++;
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("The input consists of %zu regions\n"
           "The regions are:\n", inputRegionCount);
    for (size_t i=0; i<inputRegionCount; i++)
    {
        printf("  %zux%zu: %zu %zu %zu %zu %zu %zu\n",
               inputRegions[i].m,
               inputRegions[i].n,
               inputRegions[i].shapes2fit[0],
               inputRegions[i].shapes2fit[1],
               inputRegions[i].shapes2fit[2],
               inputRegions[i].shapes2fit[3],
               inputRegions[i].shapes2fit[4],
               inputRegions[i].shapes2fit[5]);
        printf("----------------------------------------\n");
    }
#endif

    /* just brute force adding the shapes to the given area */
#define INIT_REGIONS_POSSIBLE_COUNTER       0
#define INIT_REGIONS_ITERATOR               0
    size_t regionsPossibleCounter = INIT_REGIONS_POSSIBLE_COUNTER;
    for (size_t i=INIT_REGIONS_ITERATOR; i<inputRegionCount; i++)
    {
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("Testing region %zu ... ", i);
#endif
        if (is_worth_trying(&inputRegions[i], shapeInput, SHAPE_INPUT_COUNT))
        {
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("\nWe have to brute force ... ", i);
#endif
            if (test_region(&inputRegions[i], shapeInput, SHAPE_INPUT_COUNT))
            {
#ifdef VERBOSE_DEBUG_OUTPUT
                printf("IS POSSIBLE!\n", i);
#endif
                regionsPossibleCounter++;
            }
            else
            {
#ifdef VERBOSE_DEBUG_OUTPUT
                printf("not possible\n", i);
#endif
            }
        }
#ifdef VERBOSE_DEBUG_OUTPUT
        else
        {
            printf("obviously not possible\n", i);
        }
#endif
    }

    free_rainer(shapeInput, SHAPE_INPUT_COUNT);
    free(inputRegions);

    printf("There are %lld regions which can fit all their listed presents\n", regionsPossibleCounter);
    return funcRv;
}
