#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "../day9/day9.c"

int main(int argc, char* argv[])
{
#if 0
    bool rv;
    PointType input[4];
    input[0].x = 1;
    input[0].y = 1;
    input[1].x = 1;
    input[1].y = 4;
    input[2].x = 4;
    input[2].y = 4;
    input[3].x = 4;
    input[3].y = 1;
    PointType a;
    PointType b;

    int retVal = 0;

    a.y = 0;
    b.y = 5;
    for (int64_t i=0; i<10; i++)
    {
        a.x = i;
        b.x = i;
        int64_t rv = polygonFrameIntersections(input, 2, &a, &b);
        if (rv != 0)
        {
            printf("FAILED polygonFrameIntersections expected 0 got %lld @ line (%llu,%llu) (%llu,%llu) and (%llu,%llu) (%llu,%llu)\n",
                        rv, a.x, a.y, b.x, b.y, input[0].x, input[0].y, input[1].x, input[1].y);
            retVal = -1;
        }
    }

    a.x = 0;
    b.x = 5;
    for (int64_t i=0; i<10; i++)
    {
        a.y = i;
        b.y = i;
        int64_t rv = polygonFrameIntersections(input, 2, &a, &b);
        if ((i >= 1) && (i <= 4))
        {
            if (rv != 2)
            {
                printf("FAILED polygonFrameIntersections expected 2 got %lld @ line (%llu,%llu) (%llu,%llu) and (%llu,%llu) (%llu,%llu)\n",
                            rv, a.x, a.y, b.x, b.y, input[0].x, input[0].y, input[1].x, input[1].y);
                retVal = -1;
            }
        }
        else
        {
            if (rv != 0)
            {
                printf("FAILED polygonFrameIntersections expected 0 got %lld @ line (%llu,%llu) (%llu,%llu) and (%llu,%llu) (%llu,%llu)\n",
                            rv, a.x, a.y, b.x, b.y, input[0].x, input[0].y, input[1].x, input[1].y);
                retVal = -1;
            }
        }
        
    }

    a.x = 0;
    b.x = 1;
    for (int64_t i=0; i<10; i++)
    {
        a.y = i;
        b.y = i;
        int64_t rv = polygonFrameIntersections(input, 4, &a, &b);
        if ((i >= 1) && (i <= 4))
        {
            if (rv != 1)
            {
                printf("FAILED polygonFrameIntersections expected 1 got %lld @ line (%llu,%llu) (%llu,%llu) and (%llu,%llu) (%llu,%llu)\n",
                            rv, a.x, a.y, b.x, b.y, input[0].x, input[0].y, input[1].x, input[1].y);
                retVal = -1;
            }
        }
        else
        {
            if (rv != 0)
            {
                printf("FAILED polygonFrameIntersections expected 0 got %lld @ line (%llu,%llu) (%llu,%llu) and (%llu,%llu) (%llu,%llu)\n",
                            rv, a.x, a.y, b.x, b.y, input[0].x, input[0].y, input[1].x, input[1].y);
                retVal = -1;
            }
        }
        
    }

    a.y = 5;
    b.y = 4;
    for (int64_t i=0; i<10; i++)
    {
        a.x = i;
        b.x = i;
        int64_t rv = polygonFrameIntersections(input, 4, &a, &b);
        if ((i >= 1) && (i <= 4))
        {
            if (rv != 1)
            {
                printf("FAILED polygonFrameIntersections expected 1 got %lld @ line (%llu,%llu) (%llu,%llu) and (%llu,%llu) (%llu,%llu)\n",
                            rv, a.x, a.y, b.x, b.y, input[0].x, input[0].y, input[1].x, input[1].y);
                retVal = -1;
            }
        }
        else
        {
            if (rv != 0)
            {
                printf("FAILED polygonFrameIntersections expected 0 got %lld @ line (%llu,%llu) (%llu,%llu) and (%llu,%llu) (%llu,%llu)\n",
                            rv, a.x, a.y, b.x, b.y, input[0].x, input[0].y, input[1].x, input[1].y);
                retVal = -1;
            }
        }
        
    }

    return retVal;
#else
    return 0;
#endif
}