#include <stdio.h>

void swap(int16_t* a, int16_t* b) {
    int16_t temp = *a;
    *a = *b;
    *b = temp;
}

int16_t partition(int16_t arr[], int16_t low, int16_t high) {

    // Initialize pivot to be the first element
    int16_t p = arr[low];
    int16_t i = low;
    int16_t j = high;

    while (i < j) {

        // Find the first element greater than
        // the pivot (from starting)
        while (arr[i] <= p && i <= high - 1) {
            i++;
        }

        // Find the first element smaller than
        // the pivot (from last)
        while (arr[j] > p && j >= low + 1) {
            j--;
        }
        if (i < j) {
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[low], &arr[j]);
    return j;
}

void quicksort(int16_t arr[], int16_t low, int16_t high) {
    if (low < high) {

        // call partition function to find Partition Index
        int16_t pi = partition(arr, low, high);

        // Recursively call quickSort() for left and right
        // half based on Partition Index
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}
