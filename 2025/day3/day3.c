#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../../puzzle_solver.h"

static int solve_puzzle_varbattery(FILE* input, size_t batteryLength)
{
#define MAX_BATTERY_LENGTH  128
    char batteryString[MAX_BATTERY_LENGTH];
    int64_t totalBatteryJoltage = 0; 

    while (1)
    {
        char batterySelection[MAX_BATTERY_LENGTH] = {0};
        size_t next_digit_start_pos = 0;
        char msb_c = '0';   //DEPRECATED
        size_t msb_c_pos = 0; //DEPRECATED
        char lsb_c = '0'; //DEPRECATED

        /* read battery string */
        char* rv = fgets(batteryString, MAX_BATTERY_LENGTH, input);
        if (rv == NULL)
        {
            printf("Total battery joltage: %llu\n", totalBatteryJoltage);
            return (feof(input) == 0) ? -1 : 0;
        }
        if (strnlen(batteryString, MAX_BATTERY_LENGTH) == (MAX_BATTERY_LENGTH-1))
        {
            printf("ERROR: Battery string too long!\n");
            return -1;
        }

        if (batteryString[strnlen(batteryString, MAX_BATTERY_LENGTH)-1] == '\n')
        {
            batteryString[strnlen(batteryString, MAX_BATTERY_LENGTH)-1] = '\0';
        }

        for (size_t k=0; k<batteryLength; k++)
        {
            /* search for most significant joltage digit */
            for (size_t i=next_digit_start_pos; i<=(strnlen(batteryString, MAX_BATTERY_LENGTH)-(batteryLength-k)); i++)
            {
                batteryString[i];
                if (   (batteryString[i] >= '0')
                    && (batteryString[i] <= '9'))
                {
                    if (batteryString[i] > batterySelection[k])
                    {
#ifdef VERBOSE_DEBUG_OUTPUT
                        printf("    Battery joltage digit %zu::found new max %c, previous %c\n", k, batteryString[i], batterySelection[k]);
#endif
                        batterySelection[k] = batteryString[i];
                        next_digit_start_pos = i+1;
                    }
                }
            }
        }

        /* process battery data */
        int64_t currentBatteryJoltage = 0;
        for (size_t k=0; k<batteryLength; k++)
        {
            currentBatteryJoltage += (int64_t)(batterySelection[k] - '0');
            currentBatteryJoltage *= 10LL;
        }
        currentBatteryJoltage /= 10LL;
#ifdef VERBOSE_DEBUG_OUTPUT
        printf("Battery joltage: %llu\n", currentBatteryJoltage);
#endif
        totalBatteryJoltage += currentBatteryJoltage;
    }
    
    return 0;
}

int solve_puzzle(FILE* input)
{
#if BUILD_SOLUTION == 1
    return solve_puzzle_varbattery(input, 2);
#elif BUILD_SOLUTION == 2
    return solve_puzzle_varbattery(input, 12);
#endif
}
