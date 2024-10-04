#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "float_utils.h"
#include "print_utils.h"

int main() {
  float num = 25;

  float a = 52.0;
  float b = 2.0;

  float x = fdiv2(num);

  while (fabsf(fsquare(x) - num) > 0.0000001) {
    float temp = fdiv(num, x);
    x = fadd(x, temp);
    x = fdiv2(x);
    fprint(x);
  }
  printf("Square root: %f\n", x);

  return 0;
}

// bool isPerfectSquare(int num) {
//   long x = num;
//   while (x * x > num) {
//     x = (x + num / x) / 2;
//   }
//   return x * x == num;
// }