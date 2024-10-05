#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "float_utils.h"
#include "print_utils.h"

int main() {
  float num = 49;

  float x = fdiv2(num);

  while (fcmp(fsquare(x), num) == 1) {
    float temp = fdiv(num, x);
    x = fadd(x, temp);
    x = fdiv2(x);
  }
  // while (fsquare(x) - num > 0.0001) {
  //   float temp = fdiv(num, x);
  //   x = fadd(x, temp);
  //   x = fdiv2(x);
  // }
  printf("Square root: %f\n", x);
  printf("Square root: %f\n", sqrt(num));

  return 0;
}

// bool isPerfectSquare(int num) {
//   long x = num;
//   while (x * x > num) {
//     x = (x + num / x) / 2;
//   }
//   return x * x == num;
// }