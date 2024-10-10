#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "fp32_utils.h"
#include "print_utils.h"

int condition(float a, float b) {
  b = fnegative(b);
  float sum = fadd(fsquare(a), b);
  sum = fabsf(sum);
  return fcmp(sum, 0.1) == 1;
}

int main() {
  for (float i = 1; i < 1000; i++) {
    float num = i;
    float half = fdiv2(num);

    while (condition(half, num)) {
      half = fadd(half, fdiv(num, half));
      half = fdiv2(half);
    }

    half = fp32_to_uint32(half);

    bool is_equal = (num == fsquare(half));
    if (is_equal) {
      printf("i: %f. ", half);
      printf("Is num %d a perfect square? %s\n", fp32_to_uint32(i), is_equal ? "True" : "False");
    }
  }

  return 0;
}

// bool isPerfectSquare(int num) {
//   long x = num;
//   while (x * x > num) {
//     x = (x + num / x) / 2;
//   }
//   return x * x == num;
// }