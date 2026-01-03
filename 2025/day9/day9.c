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
    int64_t x;
    int64_t y;
} PointType;

typedef struct 
{
    PointType* start;
    PointType* end;
    int64_t area;
    /* data */
} RectangleType;

#if BUILD_SOLUTION == 2
static int compareRectArea(const void* a, const void* b)
{
    return -(int)(((RectangleType*)a)->area - ((RectangleType*)b)->area);
}

static RectangleType* calcRectAreas(PointType* polyInput, size_t polyLen, size_t* rectLen)
{
    const size_t reallocSteps = 64;
    (*rectLen) = 0;
    size_t mallocdMem = reallocSteps;
    RectangleType* output = malloc(mallocdMem*sizeof(RectangleType));
    if (output == NULL)
    {
        return NULL;
    }

    for (size_t i=0; i<polyLen; i++)
    {
        for (size_t k=i+1; k<polyLen; k++)
        {
            if ((*rectLen) >= mallocdMem)
            {
                mallocdMem += reallocSteps;
                RectangleType* newOutput = realloc(output, mallocdMem*sizeof(RectangleType));
                if (newOutput == NULL)
                {
                    free(output);
                    return NULL;
                }
                output = newOutput;
            }
            output[*rectLen].start = &polyInput[i];
            output[*rectLen].end = &polyInput[k];
            int64_t currentWidth = polyInput[i].x - polyInput[k].x;
            currentWidth = (currentWidth < 0) ? -currentWidth : currentWidth;
            currentWidth++;
            int64_t currentHeight = polyInput[i].y - polyInput[k].y;
            currentHeight = (currentHeight < 0) ? -currentHeight : currentHeight;
            currentHeight++;
            output[*rectLen].area = currentWidth*currentHeight;
            (*rectLen)++;
        }
    }

    return output;
}

static void compressInput(PointType* pointInput, size_t pointInputCounter)
{
    /* find minimum x coordinate */
    int64_t minimum_x = -1;
    for (size_t i=0; i<pointInputCounter; i++)
    {
        if (   (minimum_x < 0)
            || (pointInput[i].x < minimum_x))
        {
            minimum_x = pointInput[i].x;
        }
    }

    /* find minimum y coordinate */
    int64_t minimum_y = -1;
    for (size_t i=0; i<pointInputCounter; i++)
    {
        if (   (minimum_y < 0)
            || (pointInput[i].y < minimum_y))
        {
            minimum_y = pointInput[i].y;
        }
    }

    /* subtract minimum values from each coordinate */
    for (size_t i=0; i<pointInputCounter; i++)
    {
        pointInput[i].x -= minimum_x;
        pointInput[i].y -= minimum_y;
    }
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("    Compressed x coordinates by %lld\n"
           "    Compressed x coordinates by %lld\n",
           minimum_x, minimum_y);
#endif
}

static bool isOnPolygonEdge(const PointType* input, size_t len, const PointType* pointA)
{
    for (size_t i=0; i<len; i++)
    {
        int64_t xStart = (input[i].x < input[(i+1)%len].x) ? input[i].x : input[(i+1)%len].x;
        int64_t xEnd   = (input[i].x > input[(i+1)%len].x) ? input[i].x : input[(i+1)%len].x;
        int64_t yStart = (input[i].y < input[(i+1)%len].y) ? input[i].y : input[(i+1)%len].y;
        int64_t yEnd   = (input[i].y > input[(i+1)%len].y) ? input[i].y : input[(i+1)%len].y;

        if (   (pointA->x >= xStart)
            && (pointA->x <= xEnd)
            && (pointA->y >= yStart)
            && (pointA->y <= yEnd))
        {
            return true;
        }
    }
    return false;
}

#if 0
/** only for rectangular polygons */
static int64_t polygonFrameIntersections(const PointType* poly, size_t len, const PointType* pointA, const PointType* pointB)
{
    int64_t rv = 0;
    for (size_t i=0; i<len; i++)
    {
        int64_t xStartPoly = (poly[i].x < poly[(i+1)%len].x) ? poly[i].x : poly[(i+1)%len].x;
        int64_t xEndPoly   = (poly[i].x > poly[(i+1)%len].x) ? poly[i].x : poly[(i+1)%len].x;
        int64_t yStartPoly = (poly[i].y < poly[(i+1)%len].y) ? poly[i].y : poly[(i+1)%len].y;
        int64_t yEndPoly   = (poly[i].y > poly[(i+1)%len].y) ? poly[i].y : poly[(i+1)%len].y;

        int64_t xStartLine = (pointA->x < pointB->x) ? pointA->x : pointB->x;
        int64_t xEndLine   = (pointA->x > pointB->x) ? pointA->x : pointB->x;
        int64_t yStartLine = (pointA->y < pointB->y) ? pointA->y : pointB->y;
        int64_t yEndLine   = (pointA->y > pointB->y) ? pointA->y : pointB->y;
        if (xStartPoly == xEndPoly)
        {
            /* vertical polygon line */
            if (xStartLine == xEndLine)
            {
                /* the lines are parallel, hence no intersections */
                continue;
            }
            else if (yStartLine == yEndLine)
            {
                /* count possible intersection */
                if (   (yStartLine >= yStartPoly)
                    && (yStartLine <=   yEndPoly)
                    && (xStartPoly >= xStartLine)
                    && (xStartPoly <=   xEndLine))
                {
                    rv++;
                }
            }
            else
            {
                printf("ERROR: the function polygonFrameIntersections() does not support this line\n");
                return 0;
            }
        }
        else if (yStartPoly == yEndPoly)
        {
            /* horizontal polygon line */
            if (yStartLine == yEndLine)
            {
                /* the lines are parallel, hence no intersections */
                continue;
            }
            else if (xStartLine == xEndLine)
            {
                /* count possible intersection */
                if (   (yStartPoly >= yStartLine)
                    && (yStartPoly <=   yEndLine)
                    && (xStartLine >= xStartPoly)
                    && (xStartLine <=   xEndPoly))
                {
                    rv++;
                }
            }
            else
            {
                printf("ERROR: the function polygonFrameIntersections() does not support this line\n");
                return 0;
            }
        }
        else
        {
            printf("ERROR: the function polygonFrameIntersections() does not support this polygon\n");
            return 0;
        }
    }
    return rv;
}
#endif

static bool isInsidePolygon(const PointType* input, size_t len, const PointType* pointA)
{
#if 1
    double sumOfAngels = 0.0; /* yes, i meant angels! */
    for (size_t i=0; i<len; i++)
    {
        int64_t dx1 = input[i].x - pointA->x;
        int64_t dy1 = input[i].y - pointA->y;
        int64_t dx2 = input[(i+1)%len].x - pointA->x;
        int64_t dy2 = input[(i+1)%len].y - pointA->y;
        double tempAngle1 = 0.0;
        double tempAngle2 = 0.0;
        if (dx1 == 0)
        {
            if (dy1 > 0)
            {
                tempAngle1 += M_PI_2;
            }
            else if (dy1 < 0)
            {
                tempAngle1 -= M_PI_2;
            }
            else /* if (dy1 == 0) */
            {
                return true;
            }
        }
        else
        {
            tempAngle1 = atan2((double)dy1, (double)dx1);
        }

        if (dx2 == 0)
        {
            if (dy2 > 0)
            {
                tempAngle2 += M_PI_2;
            }
            else if (dy2 < 0)
            {
                tempAngle2 -= M_PI_2;
            }
            else /* if (dy2 == 0) */
            {
                return true;
            }
        }
        else
        {
            tempAngle2 = atan2((double)dy2, (double)dx2);
        }
#if 0
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("        Temp angels for (%lld,%lld) to (%lld,%lld) and (%lld,%lld) are %f, %f\n", pointA->x, pointA->y, input[i].x, input[i].y, input[(i+1)%len].x, input[(i+1)%len].y, tempAngle1, tempAngle2);
#endif
#endif
        double anotherTempAngle = tempAngle1 - tempAngle2;
        sumOfAngels += (anotherTempAngle > M_PI) ? (anotherTempAngle - M_PI) : ((anotherTempAngle < -M_PI) ? (anotherTempAngle + M_PI) : anotherTempAngle);
    }
#if 0
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("        Sum of angels for (%lld,%lld) is %f\n", pointA->x, pointA->y, sumOfAngels);
#endif
#endif
    sumOfAngels = (sumOfAngels < 0.0) ? -sumOfAngels : sumOfAngels;
    if (sumOfAngels < M_PI_2)
    {
        return false;
    }

    return true;
#else
    PointType tempPnt;
    tempPnt.x = 0;
    tempPnt.y = pointA->y;
    if (   (isOnPolygonEdge(input, len, pointA) == true)
        || (polygonFrameIntersections(input, len, pointA, &tempPnt) % 2) == 1)
    {
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("            Point (%lld, %lld) is inside the polygon\n", pointA->x, pointA->y);
#endif
        return true;
    }
    else
    {
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("            Point (%lld, %lld) is outside the polygon\n", pointA->x, pointA->y);
#endif
        return false;
    }
    //return ((polygonFrameIntersections(input, len, pointA, &tempPnt) % 2) == 1);
#endif
}

static bool isTotallyRedGreen(const PointType* input, size_t len, const PointType* pointA, const PointType* pointB)
{
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("    Checking rectangle (%lld,%lld) (%lld,%lld) ...\n", pointA->x, pointA->y, pointB->x, pointB->y);
#endif
    int64_t xStart = (pointA->x < pointB->x) ? pointA->x : pointB->x;
    int64_t xEnd   = (pointA->x > pointB->x) ? pointA->x : pointB->x;
    int64_t yStart = (pointA->y < pointB->y) ? pointA->y : pointB->y;
    int64_t yEnd   = (pointA->y > pointB->y) ? pointA->y : pointB->y;
    PointType temp;
    temp.y = yStart;
    for (temp.x=xStart; temp.x<xEnd; temp.x++)
    {
        if (isOnPolygonEdge(input, len, &temp) == false)
        {
            if (isInsidePolygon(input, len, &temp) == false)
            {
                return false;
            }
        }
    }

    temp.x = xEnd;
    for (temp.y=yStart; temp.y<yEnd; temp.y++)
    {
        if (isOnPolygonEdge(input, len, &temp) == false)
        {
            if (isInsidePolygon(input, len, &temp) == false)
            {
                return false;
            }
        }
    }

    temp.y = yEnd;
    for (temp.x=xEnd; temp.x>xStart; temp.x--)
    {
        if (isOnPolygonEdge(input, len, &temp) == false)
        {
            if (isInsidePolygon(input, len, &temp) == false)
            {
                return false;
            }
        }
    }

    temp.x = xStart;
    for (temp.y=yEnd; temp.y>yStart; temp.y--)
    {
        if (isOnPolygonEdge(input, len, &temp) == false)
        {
            if (isInsidePolygon(input, len, &temp) == false)
            {
                return false;
            }
        }
    }
    return true;
}
#endif

int solve_puzzle(FILE* input)
{
#define MAX_INPUT_LINE_LENGTH   64
#define REALLOC_STEP_SIZE        2
    int funcRv = 0;

    char tempBuf[MAX_INPUT_LINE_LENGTH];
    size_t pointInputSize = REALLOC_STEP_SIZE;
    PointType* pointInput = malloc(sizeof(PointType)*pointInputSize);
    size_t pointInputCounter = 0;
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

        if (pointInputCounter >= pointInputSize)
        {
            pointInputSize += REALLOC_STEP_SIZE;
            PointType* newPointInput = (PointType*)realloc(pointInput, pointInputSize*sizeof(PointType));
            if (newPointInput == NULL)
            {
                funcRv = -1;
                printf("ERROR: realloc() failed!\n");
                break;
            }
            pointInput = newPointInput;
        }

        char* startptr = tempBuf;
        for (size_t i=0; i<2; i++)
        {
            char* endptr;
            (&(pointInput[pointInputCounter].x))[i] = (double)strtoul(startptr, &endptr, 10);
            if (endptr == startptr)
            {
                funcRv = -1;
                printf("ERROR: strtoul() failed to parse coordinate %zu (0=x, 1=y) for point %zu!\n", i, pointInputCounter);
                break;
            }
            startptr = endptr + 1; /* skip , delimiter */
        }
        pointInputCounter++;
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("The input consists of %zu points\n", pointInputCounter);
    printf("The points are:\n");
    for (size_t i=0; i<pointInputCounter; i++)
    {
        printf("  (%lld,%lld)\n", pointInput[i].x, pointInput[i].y);
    }
#endif

    int64_t largestAreaUpToNow = 0;

#if BUILD_SOLUTION == 1
    /* calculate */
    size_t largestAreaStartPointIndex = 0;
    size_t largestAreaEndPointIndex = 0;
    for (size_t i=0; i<pointInputCounter; i++)
    {
        printf("Processing each rectangle with point %zu...\n", i);
        for (size_t k=i+1; k<pointInputCounter; k++)
        {
            printf("    processing rectangle with points %zu,%zu...\n", i,k);
            int64_t currentWidth = pointInput[i].x - pointInput[k].x;
            currentWidth = (currentWidth < 0) ? -currentWidth : currentWidth;
            currentWidth++;
            int64_t currentHeight = pointInput[i].y - pointInput[k].y;
            currentHeight = (currentHeight < 0) ? -currentHeight : currentHeight;
            currentHeight++;
            int64_t currentArea = currentWidth*currentHeight;
            if (currentArea > largestAreaUpToNow)
            {
#ifdef VERBOSE_DEBUG_OUTPUT
                printf("        Found larger area by points %zu and %zu\n", i, k);
#endif
                largestAreaStartPointIndex = i;
                largestAreaEndPointIndex = k;
                largestAreaUpToNow = currentArea;
            }
        }
    }
#elif BUILD_SOLUTION == 2
#if 0
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("Compressing input ...\n");
#endif
    compressInput(pointInput, pointInputCounter);
#endif
    printf("Calculating area of all possible rectangles ...\n");

    size_t rectanglesCount = 0;
    RectangleType* rectangles = calcRectAreas(pointInput, pointInputCounter, &rectanglesCount);
    if (rectangles == NULL)
    {
        printf("ERROR: calcRectAreas returned NULL\n");
    }
    else
    {
        printf("    calculated %zu rectangle areas\n", rectanglesCount);
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("    areas:");
        for (size_t i=0; i<rectanglesCount; i++)
        {
            printf(" %lld", rectangles[i].area);
        }
        printf("\n");
#endif
        printf("Sorting rectangles by area (descending) ...\n");
        qsort(rectangles, rectanglesCount, sizeof(RectangleType), compareRectArea);
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("    sorted rectangle areas:");
        for (size_t i=0; i<rectanglesCount; i++)
        {
            printf(" %lld", rectangles[i].area);
        }
        printf("\n");
#endif
        printf("Searching for first rectangle that is totally inside of the polygon ...\n");
        /* skip previously checked rectangles */
        /* note: for the real input data, this algorithm takes a lot of time. I let the algorithm run for a few hours, noted where i stopped and used that number as a start value for the next run.
        For my input data to find the rectangle in an acceptable time, the start value of i was 14670 */
        for (size_t i=0; i<rectanglesCount; i++)
        {
            printf("    checking rectangle no %zu with area %lld: (%lld,%lld) (%lld,%lld) ...", i, rectangles[i].area, rectangles[i].start->x, rectangles[i].start->y, rectangles[i].end->x, rectangles[i].end->y);
            if (isTotallyRedGreen(pointInput, pointInputCounter, rectangles[i].start, rectangles[i].end) == true)
            {
                printf(" SUCCESS\n");
                largestAreaUpToNow = rectangles[i].area;
                break;
            }
            else
            {
                printf(" failed [%2.2f%%]\n", 100.0f*((double)i)/((double)rectanglesCount));
            }
        }   
    }
    free(rectangles);
#endif
    free(pointInput);

    printf("The largest rectangle has an area of %lld\n", largestAreaUpToNow);
    return funcRv;
}
