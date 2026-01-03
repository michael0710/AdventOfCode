#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "../../puzzle_solver.h"

int solve_puzzle(FILE* input)
{
#define MAX_INPUT_LINE_LENGTH   4096
    int funcRv = 0;

    char tempBuf[MAX_INPUT_LINE_LENGTH];
    unsigned long long grandTotal = 0;
#if BUILD_SOLUTION == 1
    size_t colIterator = 0;
    size_t colCount = 1;
    size_t rowIterator = 0;
    size_t rowCount = 1;
    unsigned long long* cephalopodsMatrix = malloc(sizeof(unsigned long long));
    while (funcRv == 0)
    {
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("Parsing row %zu\n", rowIterator);
#endif
        char* rv = fgets(tempBuf, MAX_INPUT_LINE_LENGTH, input);
        if (rv == NULL)
        {
            funcRv = -1;
            printf("ERROR: reached unexpected end of file!\n");
            break;
        }
        if (strnlen(tempBuf, MAX_INPUT_LINE_LENGTH) >= (MAX_INPUT_LINE_LENGTH-1))
        {
            funcRv = -1;
            printf("ERROR: Input line too long!\n");
            break;
        }

        char* charIterator = tempBuf;
        colIterator = 0;
        while ((*charIterator != '\n') && (*charIterator != '\0'))
        {
            /* initial whitespaces are ignored by strtoull */
            unsigned long long cephalopodNumber = 0;
            _set_errno(0);
            char* endptr = NULL;
            cephalopodNumber = strtoull(charIterator, &endptr, 10);
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("    Parsing column %zu\n", colIterator);
#endif
            if (endptr == charIterator)
            {
                break;
            }
            else if (errno != 0)
            {
                funcRv = -1;
                printf("ERROR: Call to strtoull failed with errno %d!\n", errno);
                break;
            }
            charIterator = endptr;
            
            if (colIterator >= colCount)
            {
                colCount++;
                unsigned long long* newMatrix = realloc(cephalopodsMatrix, sizeof(unsigned long long) * colCount * rowCount);
                if (newMatrix == NULL)
                {
                    funcRv = -1;
                    printf("ERROR: Failed to realloc memory for cephalopods matrix!\n");
                    break;
                }
                cephalopodsMatrix = newMatrix;
            }
            cephalopodsMatrix[(rowIterator * colCount) + colIterator] = cephalopodNumber;
            colIterator++;
        }
        if (colIterator == 0)
        {
            rowCount--;
            /* no more data to read */
            break;
        }

        rowIterator++;
        if (rowIterator >= rowCount)
        {
            rowCount++;
            unsigned long long* newMatrix = realloc(cephalopodsMatrix, sizeof(unsigned long long) * colCount * rowCount);
            if (newMatrix == NULL)
            {
                funcRv = -1;
                printf("ERROR: Failed to realloc memory for cephalopods matrix!\n");
                break;
            }
            cephalopodsMatrix = newMatrix;
        }
    }

#ifdef VERBOSE_DEBUG_OUTPUT
    /* print read data */
    printf("Input data:\n");
    for (size_t i=0; i<rowCount; i++)
    {
        for (size_t j=0; j<colCount; j++)
        {
            printf("%llu ", cephalopodsMatrix[i*rowCount + j]);
        }
        printf("\n");
    }
    printf("Processing homework ... rowCount=%zu, colCount=%zu\n", rowCount, colCount);
#endif
    
    char* operatorIterator = tempBuf;
    size_t operationIterator = 0;
    while ((funcRv == 0) && (*operatorIterator != '\0'))
    {
        unsigned long long operationResult = 0;
        if (*operatorIterator == ' ')
        {
            operatorIterator++;
            continue;
        }
        else if (*operatorIterator == '+')
        {
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("    Found + operator\n");
            printf("        Calculating ");
#endif
            /* addition operator */
            for (size_t i=0; i<rowCount; i++)
            {
#ifdef VERBOSE_DEBUG_OUTPUT
                printf("+ %llu ", cephalopodsMatrix[operationIterator + i*colCount]);
#endif
                operationResult += cephalopodsMatrix[operationIterator + i*colCount];
            }
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("= %llu\n", operationResult);
#endif
            operationIterator++;
        }
        else if (*operatorIterator == '*')
        {
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("    Found * operator\n");
            printf("        Calculating ");
#endif
            operationResult = 1;
            for (size_t i=0; i<rowCount; i++)
            {
#ifdef VERBOSE_DEBUG_OUTPUT
                printf("* %llu ", cephalopodsMatrix[operationIterator + i*colCount]);
#endif
                operationResult *= cephalopodsMatrix[operationIterator + i*colCount];
            }
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("= %llu\n", operationResult);
#endif
            /* multiplication operator */
            operationIterator++;
        }
        else if (*operatorIterator == '\n')
        {
            break;
        }
        else
        {
            funcRv = -1;
            printf("ERROR: Unknown operator char '%c' found!\n", *operatorIterator);
            break;
        }
        operatorIterator++;
        grandTotal += operationResult;
    }
    free(cephalopodsMatrix);
#elif BUILD_SOLUTION == 2
    char* rvFgets = fgets(tempBuf, MAX_INPUT_LINE_LENGTH, input);
    if (rvFgets == NULL)
    {
        printf("ERROR: reached unexpected end of file!\n");
        return -1;
    }
    size_t lineLength = strnlen(tempBuf, MAX_INPUT_LINE_LENGTH);
    if (lineLength >= (MAX_INPUT_LINE_LENGTH-1))
    {
        printf("ERROR: Input line too long!\n");
        return -1;
    }
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("Input has %zu lines\n", lineLength);
#endif
    size_t colLength = 0;
    char c;
    int rvFseek = fseek(input, 0, SEEK_SET);
    if (rvFseek != 0)
    {
        printf("ERROR: fseek() returned non zero value %d!\n", rvFseek);
        return -1;
    }
    while((c = fgetc(input)) != EOF)
    {
        colLength += ((c == '\n') ? 1 : 0);
    }
    rvFseek = fseek(input, -1, SEEK_END);
    if (rvFseek != 0)
    {
        printf("ERROR: fseek() returned non zero value %d!\n", rvFseek);
        return -1;
    }
    if (fgetc(input) != '\n')
    {
        /* increase the counter if the last line is not empty */
        colLength++;
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("Last line of input file is not empty\n");
#endif
    }
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("Input has %zu columns\n", colLength);
#endif
    size_t colIterator=lineLength-1;
    uint64_t currentProductResult = 1;
    uint64_t currentSumResult = 0;
    do
    {
        colIterator--;
        uint64_t currentColumnNumber = 0;
        bool hasDigits = false;
        for (size_t i=0; i<colLength; i++)
        {
            int rvFseek = fseek(input, colIterator+lineLength*i, SEEK_SET);
            if (rvFseek != 0)
            {
                printf("ERROR: fseek() returned non zero value %d!\n", rvFseek);
                return -1;
            }
            c = fgetc(input);
            if (c == ' ')
            {
                continue;
            }
            else if (c >= '0' && c <= '9')
            {
                hasDigits = true;
                currentColumnNumber = currentColumnNumber * 10 + (c - '0');
            }
            else if (c == '+')
            {
                currentSumResult += currentColumnNumber;
                grandTotal += currentSumResult;
#ifdef VERBOSE_DEBUG_OUTPUT
                printf("Column sum result is %llu\n", currentSumResult);
#endif
                currentSumResult = 0;
                currentProductResult = 1;
                hasDigits = false;
                break;
            }
            else if (c == '*')
            {
                currentProductResult *= currentColumnNumber;
                grandTotal += currentProductResult;
#ifdef VERBOSE_DEBUG_OUTPUT
                printf("Column product result is %llu\n", currentProductResult);
#endif
                currentSumResult = 0;
                currentProductResult = 1;
                hasDigits = false;
                break;
            }
            else
            {
                printf("ERROR: Unknown character 0x%02X found!\n", (unsigned int)c);
                return -1;
            }
        }
        if (hasDigits == true)
        {
            currentSumResult += currentColumnNumber;
            currentProductResult *= currentColumnNumber;
#ifdef VERBOSE_DEBUG_OUTPUT
            printf("Column %zu number is %llu\n", colIterator, currentColumnNumber);
#endif
        }

    } while (colIterator > 0);

#endif

    printf("The grand total is %llu\n", grandTotal);
    return funcRv;
}
