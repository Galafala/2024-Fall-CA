#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "fp32_utils.h"
#include "print_utils.h"

int main() {
  float num = 49;
  float error = 0.01;

  float half = fdiv2(num);

  while (fcmp(fsquare(half), num) == 1) {
    float diff = fdiv(num, half);
    half = fadd(half, diff);
    half = fdiv2(half);
  }

  printf("Square root: %f\n", half);
  printf("Square root: %f\n", sqrt(num));
  printf("The square root of %f is %f\n", num, half);

  return 0;
}

// bool isPerfectSquare(int num) {
//   long x = num;
//   while (x * x > num) {
//     x = (x + num / x) / 2;
//   }
//   return x * x == num;
// }