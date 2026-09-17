#include <stdio.h>
#include <stdint.h>

#include "common.h"

void sort_uint16(uint16_t arr[], uint16_t count)
{
    uint8_t i;
    uint8_t j;
    uint16_t value;

    for(i = 1; i < count; i++)
    {
        value = arr[i];
        j = i;

        while((j > 0) && (arr[j - 1] > value))
        {
            arr[j] = arr[j - 1];
            j--;
        }

        arr[j] = value;
    }
}

void update_sorted(uint16_t sorted[], uint16_t oldValue, uint16_t newValue, uint16_t count)
{
    uint8_t i;

    /* Find the old value in the sorted array */
    for(i = 0; i < count; i++)
    {
        if(sorted[i] == oldValue)
            break;
    }

    /* Safety check - oldValue should always be found */
    if(i >= count)
        return;

    if(newValue > oldValue)
    {
        /*
         * New value belongs further to the right.
         * Shift smaller values one position left.
         */
        while((i < (count - 1)) &&
              (sorted[i + 1] < newValue))
        {
            sorted[i] = sorted[i + 1];
            i++;
        }
    }
    else if(newValue < oldValue)
    {
        /*
         * New value belongs further to the left.
         * Shift larger values one position right.
         */
        while((i > 0) &&
              (sorted[i - 1] > newValue))
        {
            sorted[i] = sorted[i - 1];
            i--;
        }
    }

    /* Insert new value */
    sorted[i] = newValue;
}