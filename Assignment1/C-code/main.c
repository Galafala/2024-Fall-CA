#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bfp16_utils.h"
#include "print_utils.h"

int seed(bf16 x, bf16 n, bf16 e) {
  n.s = 0x8000;
  n.bits &= 0x7FFF;
  n.bits |= 0x8000;
  bf16 added = bf16add(bf16square(x), n);
  added.s = 0;
  added.bits &= 0x7FFF;
  return bf16cmp(added, e);
}

int main() {
  float num = 88;
  float error = 0.5;

  bf16 n = fp32_to_bf16(*(uint32_t*)&num);

  bf16 x = bf16div2(n);

  bf16 e = fp32_to_bf16(*(uint32_t*)&error);

  // bf16 y = bf16square(x);

  // float k = bf16_to_fp32(y);
  // printf("Square: %f\n", k);
  // fprint(num * num * 0.25);

  while (seed(x, n, e) == 1) {
    bf16 temp = bf16div(n, x);
    x = bf16add(x, temp);
    x = bf16div2(x);
  }

  float a = bf16_to_fp32(x);

  printf("[Important] Square root: %f\n", a);
  printf("Is perfect square: %s\n", bf16cmp(bf16square(x), n) == 0 ? "true" : "false");
  printf("Square root by built-in function: %f\n", sqrt(num));

  return 0;
}

// bool isPerfectSquare(int num) {
//   long x = num;
//   while (x * x > num) {
//     x = (x + num / x) / 2;
//   }
//   return x * x == num;
// }