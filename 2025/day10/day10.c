#include <errno.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#if BUILD_SOLUTION == 2
    #include "hmath.h"
    #define ABS(x)                             (((x) < 0) ? -(x) : (x))
#endif

#include "../../puzzle_solver.h"

typedef uint16_t RegisterType;

typedef struct
{
    RegisterType indicatorPattern2Start;
    size_t switchCount;
    RegisterType* switches;
    size_t batteryCount;
    RegisterType* batteries;
} MachineType;

#if BUILD_SOLUTION == 2

static int anotherRecursiveFucker(MatrixType* A,
                                  MatrixType* b,
                                  MatrixType* x,
                                  MatrixType* isFree,
                                  CoeffType*  currentMin,
                                  size_t      freeVar2Use)
{
    const CoeffType varStartVal = 0.0;
    const CoeffType varIncVal = 1.0;
    /* search for 'freeVar2Use'st/nd/rd/th free variable in isFree vector */
    bool foundIndex = false;
    size_t freeVar2WorkWith = freeVar2Use;
    size_t i;
    for (i=0; i<isFree->m; i++)
    {
        if (isFree->a[i] != 0.0)
        {
            if (freeVar2WorkWith == 0)
            {
                foundIndex = true;
                break;
            }
            freeVar2WorkWith--; 
        }
    }
    if (foundIndex == false)
    {
        bool previousWasValid = true;
        for (size_t k=0; k<(x->m); k++)
        {
            if ((x->a)[k] < 0.0)
            {
                previousWasValid = false;
                break;
            }
        }
        hmath_get_lineq_solution(A, b, x, isFree);
        CoeffType tempSum = 0.0;
        for (size_t k=0; k<(x->m); k++)
        {
            if ((ABS((x->a)[k] - round((x->a)[k])) > 0.001))
            {
#if 1
#ifdef VERBOSE_DEBUG_OUTPUT
                printf("Discarded vector content which had non-integer elements:");
                for (size_t k=0; k<(x->m); k++)
                {
                    printf(" %6.2f", (x->a)[k]);
                }
                printf("\n");
#endif
#endif
                return 0;
            }
            tempSum += (x->a)[k];
            tempSum = round(tempSum);
        }
        if (   (tempSum < (*currentMin))
            || ((*currentMin) == 0.0))
        {
            bool valid = true;
            for (size_t k=0; k<(x->m); k++)
            {
                if ((x->a)[k] < 0.0)
                {
                    valid = false;
                    break;
                }
            }
            if (valid == true)
            {
#ifdef VERBOSE_DEBUG_OUTPUT
                printf("Found new minimum with vector content which gave %6.2f:", tempSum);
                for (size_t k=0; k<(x->m); k++)
                {
                    printf(" %6.2f", (x->a)[k]);
                }
                printf("\n");
#endif
                (*currentMin) = tempSum;
            }
            else
            {
#if 1
#ifdef VERBOSE_DEBUG_OUTPUT
                printf("Discarded vector content after validity check");
                for (size_t k=0; k<(x->m); k++)
                {
                    printf(" %6.2e", (x->a)[k]);
                }
                printf("\n");
#endif
#endif
                if (previousWasValid == true)
                {
                    return -1;
                }
            }

            return 0;
        }
        else
        {
#if 1
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("Discarded vector content which gave %6.2f:", tempSum);
            for (size_t k=0; k<(x->m); k++)
            {
                printf(" %6.2f", (x->a)[k]);
            }
            printf("\n");
#endif
#endif
            return 0;
        }
    }
    else
    {
        bool leftByBreak = false;
        for (size_t k=0; k<300; k++)
        {
            (x->a)[i] = round((CoeffType)k * varIncVal + varStartVal);
            if (anotherRecursiveFucker(A, b, x, isFree, currentMin, freeVar2Use+1) != 0)
            {
                leftByBreak = true;
                break;
            }
        }
    }

    return 0;
}

#define FLOAT_PRINTS        "e"
static void printLineq(MatrixType* A, MatrixType* B)
{
    const int PRINT_OFFSET = 8;
    const int NUMBER_WIDTH = 13;
    const int INTER_MATRIX_SPACE = 4;
    const int PRINT_PRECISION = 4;

    printf("%*s/", PRINT_OFFSET, "");
    for (size_t col=0; col<(A->n); col++)
    {
        printf("%*s", NUMBER_WIDTH, "");
    }
    printf("\\%*s/", INTER_MATRIX_SPACE, "");
    for (size_t col=0; col<(B->n); col++)
    {
        printf("%*s", NUMBER_WIDTH, "");
    }
    printf("\\\n");

    size_t maxRows = (A->m) > (B->m) ? (A->m) : (B->m);

    for (size_t row=0; row<maxRows; row++)
    {
        printf("%*s|", PRINT_OFFSET, "");
        if (row < (A->m))
        {
            for (size_t col=0; col<(A->n); col++)
            {
                printf("%*.*"FLOAT_PRINTS"", NUMBER_WIDTH, PRINT_PRECISION, (A->a)[row*(A->n)+col]);
            }
        }
        else
        {
            for (size_t col=0; col<(A->n); col++)
            {
                printf("%*s", NUMBER_WIDTH, "");
            }
        }
        printf("|%*s|", INTER_MATRIX_SPACE, "");
        if (row < (B->m))
        {
            for (size_t col=0; col<(B->n); col++)
            {
                printf("%*.*"FLOAT_PRINTS"", NUMBER_WIDTH, PRINT_PRECISION, (B->a)[row*(B->n)+col]);
            }
        }
        else
        {
            for (size_t col=0; col<(B->n); col++)
            {
                printf("%*s", NUMBER_WIDTH, "");
            }
        }
        printf("|\n");
    }
    printf("%*s\\", PRINT_OFFSET, "");
    for (size_t col=0; col<(A->n); col++)
    {
        printf("%*s", NUMBER_WIDTH, "");
    }
    printf("/%*s\\", INTER_MATRIX_SPACE, "");
    for (size_t col=0; col<(B->n); col++)
    {
        printf("%*s", NUMBER_WIDTH, "");
    }
    printf("/\n");
}
#endif

bool recursiveFucker(const MachineType* machine, RegisterType activePattern, size_t switch2start, size_t recursionDepth)
{
    if (recursionDepth  < 1)
    {
        return false;
    }
    if (recursionDepth == 1)
    {
        for (size_t i=switch2start; i<machine->switchCount; i++)
        {
#if 0
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("%*sswitch %zu...\n", (18-2*recursionDepth), " ", i);
#endif
#endif
            RegisterType workPattern = activePattern;
            workPattern ^= machine->switches[i];
            if (workPattern == 0)
            {
                return true;
            }
        }
        return false;
    }
    else
    {
        for (size_t i=switch2start; i<machine->switchCount; i++)
        {
#if 0
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("%*sChecking switch %zu against...\n", (18-2*recursionDepth), " ", i);
#endif
#endif
            RegisterType workPattern = activePattern;
            workPattern ^= machine->switches[i];
            if(recursiveFucker(machine, workPattern, i+1, recursionDepth-1))
            {
                return true;
            }
        }
        return false;
    }
}

bool puzzleSolvable(MachineType* machine, size_t switchesUsed)
{
    return recursiveFucker(machine, machine->indicatorPattern2Start, 0, switchesUsed);
}

void free_rainer(MachineType* machines, size_t count)
{
    for (size_t i=0; i<count; i++)
    {
        if (machines != NULL)
        {
            free(machines[i].switches);
            machines[i].switchCount = 0;
        }
    }

    for (size_t i=0; i<count; i++)
    {
        if (machines != NULL)
        {
            free(machines[i].batteries);
            machines[i].batteryCount = 0;
        }
    }
}

int solve_puzzle(FILE* input)
{
#define MAX_INPUT_LINE_LENGTH   256
#define REALLOC_STEP_SIZE        2
    int funcRv = 0;
    char tempBuf[MAX_INPUT_LINE_LENGTH];
    size_t allocedMachineStorage = REALLOC_STEP_SIZE;
    MachineType* machines = (MachineType*)malloc(sizeof(MachineType)*allocedMachineStorage);
    size_t machineCounter = 0;
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

        if (machineCounter >= allocedMachineStorage)
        {
            allocedMachineStorage += REALLOC_STEP_SIZE;
            MachineType* newMachines = (MachineType*)realloc(machines, allocedMachineStorage*sizeof(MachineType));
            if (newMachines == NULL)
            {
                funcRv = -1;
                printf("ERROR: realloc() failed!\n");
                break;
            }
            machines = newMachines;
        }

        machines[machineCounter].indicatorPattern2Start = 0x0;
        machines[machineCounter].switchCount = 0;
        machines[machineCounter].switches = NULL;
        machines[machineCounter].batteryCount = 0;
        machines[machineCounter].batteries = NULL;

        /* read indicatorPattern2Start*/
        size_t tempBufIterator = 0;
        for (size_t i=0; tempBufIterator<strnlen(tempBuf, MAX_INPUT_LINE_LENGTH); tempBufIterator++)
        {
            if (tempBuf[tempBufIterator] == '[')
            {
                i=0;
                continue;
            }
            else if (tempBuf[tempBufIterator] == ']')
            {
                tempBufIterator++;
                break;
            }
            else if (tempBuf[tempBufIterator] == '.')
            {
                i++;
            }
            else if (tempBuf[tempBufIterator] == '#')
            {
                machines[machineCounter].indicatorPattern2Start |= (1 << i);
                i++;
            }
            else
            {
                funcRv = -1;
                printf("ERROR: invalid character '%c' found in indicatorPattern2Start!\n", tempBuf[tempBufIterator]);
                break;
            }
        }
        if (funcRv != 0)
        {
            break;
        }

        /* read switches */
        RegisterType tempSwitchWiring = 0;
        for (; tempBufIterator<strnlen(tempBuf, MAX_INPUT_LINE_LENGTH); tempBufIterator++)
        {
            if (tempBuf[tempBufIterator] == ' ')
            {
                continue;
            }
            else if (tempBuf[tempBufIterator] == '(')
            {
                machines[machineCounter].switchCount++;
                RegisterType* newSwitches = realloc(machines[machineCounter].switches, machines[machineCounter].switchCount*sizeof(RegisterType));
                if (newSwitches == NULL)
                {
                    funcRv = -1;
                    printf("ERROR: realloc() for switches of machine %zu failed!\n", machineCounter);
                    break;
                }
                machines[machineCounter].switches = newSwitches;
                machines[machineCounter].switches[machines[machineCounter].switchCount-1] = 0x0;
                tempSwitchWiring = 0;
            }
            else if ((tempBuf[tempBufIterator] >= '0') && (tempBuf[tempBufIterator] <= '9'))
            {
                tempSwitchWiring *= 10;
                tempSwitchWiring += tempBuf[tempBufIterator] - '0';
            }
            else if ((tempBuf[tempBufIterator] == ',') || (tempBuf[tempBufIterator] == ')'))
            {
                machines[machineCounter].switches[machines[machineCounter].switchCount-1] |= (1 << tempSwitchWiring);
                tempSwitchWiring = 0;
            }
            else if (tempBuf[tempBufIterator] == '{')
            {
                break;
            }
            else
            {
                funcRv = -1;
                printf("ERROR: invalid character '%c' found in switches!\n", tempBuf[tempBufIterator]);
                break;
            }
        }
        if (funcRv != 0)
        {
            break;
        }
        
        /* read joltage */
        for (; tempBufIterator<strnlen(tempBuf, MAX_INPUT_LINE_LENGTH); tempBufIterator++)
        {
            if (tempBuf[tempBufIterator] == ' ')
            {
                continue;
            }
            else if (   (tempBuf[tempBufIterator] == '{')
                     || (tempBuf[tempBufIterator] == ','))
            {
                machines[machineCounter].batteryCount++;
                RegisterType* newBatteries = realloc(machines[machineCounter].batteries, machines[machineCounter].batteryCount*sizeof(RegisterType));
                if (newBatteries == NULL)
                {
                    funcRv = -1;
                    printf("ERROR: realloc() for batteries of machine %zu failed!\n", machineCounter);
                    break;
                }
                machines[machineCounter].batteries = newBatteries;
                machines[machineCounter].batteries[machines[machineCounter].batteryCount-1] = 0;
            }
            else if ((tempBuf[tempBufIterator] >= '0') && (tempBuf[tempBufIterator] <= '9'))
            {
                machines[machineCounter].batteries[machines[machineCounter].batteryCount-1] *= 10;
                machines[machineCounter].batteries[machines[machineCounter].batteryCount-1] += tempBuf[tempBufIterator] - '0';
            }
            else if (tempBuf[tempBufIterator] == '}')
            {
                break;
            }
            else
            {
                funcRv = -1;
                printf("ERROR: invalid character '%c' found in switches!\n", tempBuf[tempBufIterator]);
                break;
            }
        }
        if (funcRv != 0)
        {
            break;
        }

        machineCounter++;
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    printf("The input consists of %zu machines\n", machineCounter);
    printf("The machines are:\n");
    for (size_t i=0; i<machineCounter; i++)
    {
        printf("    [0x%02X]", (int)machines[i].indicatorPattern2Start);
        for (size_t k=0; k<machines[i].switchCount; k++)
        {
            printf(" (0x%02X)", (int)machines[i].switches[k]);
        }
        printf("  {");
        for (size_t k=0; k<machines[i].batteryCount; k++)
        {
            printf(" %d ", (int)machines[i].batteries[k]);
        }
        printf("}\n");
    }
#endif

    size_t sumOfAllButtonPresses = 0;
#if BUILD_SOLUTION == 1
    /* it must be determined what the fewest amount of button presses is. As a
       button press can be performed by XORING the currently active pattern
       with the switch pattern and XORING is a linear operation, each switch is
       at most pressed once.
       As we are searching for the least amount of button presses, we start
       using one button at a time. If that doesn't work out we use two buttons
       at a time until we finally reach the end if each button must be pressed.
       As XORING is a linear operation, we don't need to care about the order
       of the XORING */
    for (size_t i=0; i<machineCounter; i++)
    {
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("Calculating button presses of machine %zu ...", i);
#endif
        bool leftByBreak = false;
        for (size_t k=1; k<=machines[i].switchCount; k++)
        {
            if (puzzleSolvable(&machines[i], k))
            {
                sumOfAllButtonPresses += k;
#ifdef VERBOSE_DEBUG_OUTPUT
                printf(" %zu presses\n", k);
#endif
                leftByBreak = true;
                break;
            }
        }
        if (leftByBreak != true)
        {
            printf(" ERROR no solution possible\n");
        }
    }
#elif BUILD_SOLUTION == 2
    /* now for the part 2 we must solve a system of linear equations. Here the
       matrix columns are the number of switches and the matrix rows are the
       "joltage counters" that are increased by each button press. The matrix
       contains only ones and zeroes. If one entry of the matrix is a one, the
       related switch increases the specific joltage counter if pressed. If the
       entry of the matrix is a zero, a button press does not affect this
       specific joltage counter. */
    for (size_t i=0; i<machineCounter; i++)
    {
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("Calculating button presses of machine %zu ...", i);
#endif
        /* create matrix:
            m = switchCount
            n = batteryCount */
        CoeffType* matContent = (CoeffType*)malloc(sizeof(CoeffType)*
                machines[i].batteryCount*
                machines[i].switchCount);
        CoeffType* bContent  = (CoeffType*)malloc(sizeof(CoeffType)*
                machines[i].batteryCount);
        CoeffType* xContent = (CoeffType*)malloc(sizeof(CoeffType)*
                machines[i].switchCount);
        CoeffType* isFreeContent = (CoeffType*)malloc(sizeof(CoeffType)*
                machines[i].switchCount);
        if (   (matContent == NULL)
            || (bContent == NULL)
            || (xContent == NULL)
            || (isFreeContent == NULL))
        {
            funcRv = -1;
            printf("ERROR malloc() failed for at least one of the multiple mallocs of machine %zu\n", i);
            printf("ERROR malloc() failed: batteryCount=%zu\n", machines[i].batteryCount);
            printf("ERROR malloc() failed: switchCount=%zu\n", machines[i].batteryCount);
            free(matContent);
            free(bContent);
            free(xContent);
            free(isFreeContent);
            break;
        }

        MatrixType A =
        {
            .a = matContent,
            .m = machines[i].batteryCount,
            .n = machines[i].switchCount,
        };

        MatrixType B =
        {
            .a = bContent,
            .m = machines[i].batteryCount,
            .n = 1,
        };

        MatrixType x =
        {
            .a = xContent,
            .m = machines[i].switchCount,
            .n = 1,
        };

        MatrixType isFree =
        {
            .a = isFreeContent,
            .m = machines[i].switchCount,
            .n = 1,
        };

#ifdef VERBOSE_DEBUG_OUTPUT
        printf("    Filling matrix A (%zu,%zu) ...\n", machines[i].batteryCount, machines[i].switchCount);
#endif
        for (size_t j=0; j<machines[i].switchCount; j++)
        {
            for (size_t k=0; k<machines[i].batteryCount; k++)
            {
                A.a[A.n*k+j] = (double)((machines[i].switches[j] >> k) & 0x01);
            }
        }

#ifdef VERBOSE_DEBUG_OUTPUT
        printf("    Filling vector b ...\n");
#endif
        for (size_t k=0; k<B.m; k++)
        {
            B.a[k] = (CoeffType)machines[i].batteries[k];
        }

#ifdef VERBOSE_DEBUG_OUTPUT
        printf("    Filling vector x ...\n");
#endif
        for (size_t k=0; k<x.m; k++)
        {
            x.a[k] = (CoeffType)0.0;
        }

#ifdef VERBOSE_DEBUG_OUTPUT
        printf("    Filling vector isFree ...\n");
#endif
        for (size_t k=0; k<isFree.m; k++)
        {
            isFree.a[k] = (CoeffType)0.0;
        }

        /* solve system of linear equations (the batteries vector is the result
           vector) */
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("    Solving A*x=b with independent x values are 0.0 ...\n");
        printLineq(&A, &B);
#endif
        hmath_get_lineq_solution(&A, &B, &x, &isFree);
#ifdef VERBOSE_DEBUG_OUTPUT
        printLineq(&A, &B);
#endif
        /* find the minimum non-negative value possible for each free variable */
        CoeffType currentAbsolutMin = 0;
        anotherRecursiveFucker(&A, &B, &x, &isFree, &currentAbsolutMin, 0);
#if defined(VERBOSE_DEBUG_OUTPUT) || defined(INFO_DEBUG_OUTPUT)
        printf("    Machine %zu needs %.2f button presses\n", i, currentAbsolutMin);
#endif

        sumOfAllButtonPresses += ((size_t)round(currentAbsolutMin));
        
        free(matContent);
        free(bContent);
        free(xContent);
        free(isFreeContent);
    }
#endif

    free_rainer(machines, machineCounter);
    free(machines);
    machines = NULL;

    printf("The fewest button presses necessary are %zu\n", sumOfAllButtonPresses);
    return funcRv;
}
