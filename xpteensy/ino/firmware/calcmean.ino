#include <stdio.h>

int16_t calcmean(int16_t arr[], int16_t low, int16_t high) {

  int sum = 0;
  int cnt = 0;
  int16_t average;
  for (int i = low; i <= high; i++) {
    if (arr[i] != INITVAL) {
      sum += arr[i];
      cnt++;
    }
  }

  if (cnt > 0) {
    average = sum / cnt;
  } else {
    average = INITVAL;
  }

  return average;
}