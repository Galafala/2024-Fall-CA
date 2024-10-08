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

  fp32 fp = unpack_fp32(*(uint32_t*)&x);
  bfp16 bfp = unpack_bfp16(*(uint32_t*)&x);

  printBinary32(pack_fp32(fp));
  printBinary32(pack_bfp16(bfp));

  // while (fcmp(fsquare(x), num) == 1) {
  //   float temp = fdiv(num, x);
  //   x = fadd(x, temp);
  //   x = fdiv2(x);
  // }

  // while (fsquare(x) - num > 0.0001) {
  //   float temp = fdiv(num, x);
  //   x = fadd(x, temp);
  //   x = fdiv2(x);
  // }

  printf("Square root: %f\n", x);
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