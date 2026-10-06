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

#define SORT2(a, b)           \
do {                          \
    if ((a) > (b))            \
    {                         \
        uint16_t t = (a);     \
        (a) = (b);            \
        (b) = t;              \
    }                         \
} while (0)

void sort_uint16_7(uint16_t a[7])
{
    SORT2(a[0], a[6]);
    SORT2(a[2], a[3]);
    SORT2(a[4], a[5]);

    SORT2(a[0], a[2]);
    SORT2(a[1], a[4]);
    SORT2(a[3], a[6]);

    SORT2(a[0], a[1]);
    SORT2(a[2], a[5]);
    SORT2(a[3], a[4]);

    SORT2(a[1], a[2]);
    SORT2(a[4], a[6]);

    SORT2(a[2], a[3]);
    SORT2(a[4], a[5]);

    SORT2(a[1], a[2]);
    SORT2(a[3], a[4]);
    SORT2(a[5], a[6]);

    SORT2(a[2], a[3]);
    SORT2(a[4], a[5]);
}
