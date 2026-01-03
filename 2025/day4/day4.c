#include <stdio.h>
#include <stdbool.h>

#include "../../puzzle_solver.h"

void printAccessiblePaperRolls(bool* paperRollAccessible, size_t length, size_t lineLength)
{
    size_t row = 0;
    size_t column = 0;
    printf("Accessible paper rolls at positions:\n");
    for (size_t i=0; i<length; i++) {
        if (column == lineLength)
        {
            column = 0;
            row++;
            printf("\n");
        }
        else
        {
            printf("%d", (int)paperRollAccessible[i]);
            column++;
        }
    }
    printf("\n");
}

bool leftNeighbourExists(size_t currentColumn, size_t currentRow, size_t lineLength, size_t bytesRead)
{
    if (currentColumn <= 0)
    {
        return false;
    }
    return true;
}

bool rightNeighbourExists(size_t currentColumn, size_t currentRow, size_t lineLength, size_t bytesRead)
{
    if (currentColumn >= (lineLength - 1))
    {
        return false;
    }
    return true;
}

bool upperNeighbourExists(size_t currentColumn, size_t currentRow, size_t lineLength, size_t bytesRead)
{
    if (currentRow <= 0)
    {
        return false;
    }
    return true;
}

bool lowerNeighbourExists(size_t currentColumn, size_t currentRow, size_t lineLength, size_t bytesRead)
{
    if (currentRow*lineLength < bytesRead)
    {
        return true;
    }
    return false;
}

bool upperLeftNeighbourExists(size_t currentColumn, size_t currentRow, size_t lineLength, size_t bytesRead)
{
    return leftNeighbourExists(currentColumn, currentRow, lineLength, bytesRead) &&
           upperNeighbourExists(currentColumn, currentRow, lineLength, bytesRead);
}

bool upperRightNeighbourExists(size_t currentColumn, size_t currentRow, size_t lineLength, size_t bytesRead)
{
    return rightNeighbourExists(currentColumn, currentRow, lineLength, bytesRead) &&
           upperNeighbourExists(currentColumn, currentRow, lineLength, bytesRead);
}

bool lowerLeftNeighbourExists(size_t currentColumn, size_t currentRow, size_t lineLength, size_t bytesRead)
{
    return leftNeighbourExists(currentColumn, currentRow, lineLength, bytesRead) &&
           lowerNeighbourExists(currentColumn, currentRow, lineLength, bytesRead);
}

bool lowerRightNeighbourExists(size_t currentColumn, size_t currentRow, size_t lineLength, size_t bytesRead)
{
    return rightNeighbourExists(currentColumn, currentRow, lineLength, bytesRead) &&
           lowerNeighbourExists(currentColumn, currentRow, lineLength, bytesRead);
}

int solve_puzzle(FILE* input)
{
#define MAX_INPUT_LENGTH 32768
    char fileInput[MAX_INPUT_LENGTH];
    size_t bytesRead = fread(fileInput, 1, MAX_INPUT_LENGTH, input);
    if (ferror(input)) {
        printf("ERROR: Failed to read input file!");
        return -1;
    }

    if (bytesRead == MAX_INPUT_LENGTH) {
        printf("ERROR: Input file too large!");
        return -1;
    }

    /* determine the length of a single line including the new line character */
    size_t lineLength;
    for (lineLength=0; lineLength < bytesRead; lineLength++) {
        if (fileInput[lineLength] == '\n') {
            lineLength++;
            break;
        }
    }
#ifdef VERBOSE_DEBUG_OUTPUT
    printf("File has a line length of %zu\n"
           "File has a total size of %zu bytes\n",
           lineLength, bytesRead);
#endif

    size_t accessiblePaperRolls = 0;
    bool finished;
    do
    {
        bool paperRollAccessible[MAX_INPUT_LENGTH] = { false };
        size_t currentColumn;
        size_t currentRow;
        for (currentColumn=0; currentColumn < lineLength; currentColumn++) {
            for (currentRow=0; currentRow * lineLength + currentColumn < bytesRead; currentRow++) {
                /* calculate the adjacent paperrolls */
                /* the adjacent paper rolls are located at the relative positions
                (-1, -1), (0, -1), (1, -1), (-1, 0), (1, 0), (-1, 1), (0, 1), (1, 1) */
                if (fileInput[currentRow * lineLength + currentColumn] == '\x40')
                {
                    size_t adjacentPaperRolls = 0;
                    size_t currentIndex = currentRow * lineLength + currentColumn;
                    if (upperLeftNeighbourExists(currentColumn, currentRow, lineLength, bytesRead))
                    {
#ifdef VERBOSE_DEBUG_OUTPUT
                        printf("        Paper roll at index %3zu (row %2zu, column %2zu) has upper-left neighbour\n",
                            currentIndex, currentRow, currentColumn);
#endif
                        size_t neighbourIndex = (currentRow - 1) * lineLength + (currentColumn - 1);
                        if (fileInput[neighbourIndex] == '\x40')
                        {
#ifdef VERBOSE_DEBUG_OUTPUT
                            printf("    Paper roll at index %3zu (row %2zu, column %2zu) has upper-left adjacent paper roll\n",
                                    currentIndex, currentRow, currentColumn);
#endif
                            adjacentPaperRolls++;
                        }
                    }

                    if (upperNeighbourExists(currentColumn, currentRow, lineLength, bytesRead))
                    {
#ifdef VERBOSE_DEBUG_OUTPUT
                        printf("        Paper roll at index %3zu (row %2zu, column %2zu) has upper neighbour\n",
                            currentIndex, currentRow, currentColumn);
#endif
                        size_t neighbourIndex = (currentRow - 1) * lineLength + currentColumn;
                        if (fileInput[neighbourIndex] == '\x40')
                        {
#ifdef VERBOSE_DEBUG_OUTPUT
                            printf("    Paper roll at index %3zu (row %2zu, column %2zu) has upper adjacent paper roll\n",
                                    currentIndex, currentRow, currentColumn);
#endif
                            adjacentPaperRolls++;
                        }
                    }
                    if (upperRightNeighbourExists(currentColumn, currentRow, lineLength, bytesRead))
                    {
#ifdef VERBOSE_DEBUG_OUTPUT
                        printf("        Paper roll at index %3zu (row %2zu, column %2zu) has upper-right neighbour\n",
                            currentIndex, currentRow, currentColumn);
#endif
                        size_t neighbourIndex = (currentRow - 1) * lineLength + (currentColumn + 1);
                        if (fileInput[neighbourIndex] == '\x40')
                        {
#ifdef VERBOSE_DEBUG_OUTPUT
                            printf("    Paper roll at index %3zu (row %2zu, column %2zu) has upper-right adjacent paper roll\n",
                                    currentIndex, currentRow, currentColumn);
#endif
                            adjacentPaperRolls++;
                        }
                    }
                    if (leftNeighbourExists(currentColumn, currentRow, lineLength, bytesRead))
                    {
#ifdef VERBOSE_DEBUG_OUTPUT
                        printf("        Paper roll at index %3zu (row %2zu, column %2zu) has left neighbour\n",
                            currentIndex, currentRow, currentColumn);
#endif
                        size_t neighbourIndex = currentRow * lineLength + (currentColumn - 1);
                        if (fileInput[neighbourIndex] == '\x40')
                        {
#ifdef VERBOSE_DEBUG_OUTPUT
                            printf("    Paper roll at index %3zu (row %2zu, column %2zu) has left adjacent paper roll\n",
                                    currentIndex, currentRow, currentColumn);
#endif
                            adjacentPaperRolls++;
                        }
                    }
                    if (rightNeighbourExists(currentColumn, currentRow, lineLength, bytesRead))
                    {
#ifdef VERBOSE_DEBUG_OUTPUT
                        printf("        Paper roll at index %3zu (row %2zu, column %2zu) has right neighbour\n",
                            currentIndex, currentRow, currentColumn);
#endif
                        size_t neighbourIndex = currentRow * lineLength + (currentColumn + 1);
                        if (fileInput[neighbourIndex] == '\x40')
                        {
#ifdef VERBOSE_DEBUG_OUTPUT
                            printf("    Paper roll at index %3zu (row %2zu, column %2zu) has right adjacent paper roll\n",
                                    currentIndex, currentRow, currentColumn);
#endif
                            adjacentPaperRolls++;
                        }
                    }
                    if (lowerLeftNeighbourExists(currentColumn, currentRow, lineLength, bytesRead))
                    {
#ifdef VERBOSE_DEBUG_OUTPUT
                        printf("        Paper roll at index %3zu (row %2zu, column %2zu) has lower-left neighbour\n",
                            currentIndex, currentRow, currentColumn);
#endif
                        size_t neighbourIndex = (currentRow + 1) * lineLength + (currentColumn - 1);
                        if (fileInput[neighbourIndex] == '\x40')
                        {
#ifdef VERBOSE_DEBUG_OUTPUT
                            printf("    Paper roll at index %3zu (row %2zu, column %2zu) has lower-left adjacent paper roll\n",
                                    currentIndex, currentRow, currentColumn);
#endif
                            adjacentPaperRolls++;
                        }
                    }
                    if (lowerNeighbourExists(currentColumn, currentRow, lineLength, bytesRead))
                    {
#ifdef VERBOSE_DEBUG_OUTPUT
                        printf("        Paper roll at index %3zu (row %2zu, column %2zu) has lower neighbour\n",
                            currentIndex, currentRow, currentColumn);
#endif
                        size_t neighbourIndex = (currentRow + 1) * lineLength + currentColumn;
                        if (fileInput[neighbourIndex] == '\x40')
                        {
#ifdef VERBOSE_DEBUG_OUTPUT
                            printf("    Paper roll at index %3zu (row %2zu, column %2zu) has lower adjacent paper roll\n",
                                    currentIndex, currentRow, currentColumn);
#endif
                            adjacentPaperRolls++;
                        }
                    }
                    if (lowerRightNeighbourExists(currentColumn, currentRow, lineLength, bytesRead))
                    {
#ifdef VERBOSE_DEBUG_OUTPUT
                        printf("        Paper roll at index %3zu (row %2zu, column %2zu) has lower-right neighbour\n",
                            currentIndex, currentRow, currentColumn);
#endif
                        size_t neighbourIndex = (currentRow + 1) * lineLength + (currentColumn + 1);
                        if (fileInput[neighbourIndex] == '\x40')
                        {
#ifdef VERBOSE_DEBUG_OUTPUT
                            printf("    Paper roll at index %3zu (row %2zu, column %2zu) has lower-right adjacent paper roll\n",
                                    currentIndex, currentRow, currentColumn);
#endif
                            adjacentPaperRolls++;
                        }
                    }
#ifdef VERBOSE_DEBUG_OUTPUT
                    printf("Paper roll at index %3zu (row %2zu, column %2zu) has %zu adjacent paper rolls\n",
                            currentIndex, currentRow, currentColumn, adjacentPaperRolls);
#endif
                    if (adjacentPaperRolls < 4)
                    {
                        paperRollAccessible[currentIndex] = true;
                    }
                }
            }
        }

        finished = true;
        for (size_t i=0; i<MAX_INPUT_LENGTH; i++) {
            if (paperRollAccessible[i])
            {
                accessiblePaperRolls++;
                fileInput[i] = 'x';
#if BUILD_SOLUTION == 2
                finished = false;
#endif
            }
        }
    } while (finished == false);

#ifdef VERBOSE_DEBUG_OUTPUT
        printAccessiblePaperRolls(paperRollAccessible, bytesRead, lineLength);
#endif
    printf("Accessible paper rolls: %zu\n", accessiblePaperRolls);

    return 0;
}
