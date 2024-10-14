#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"

uint64_t count_x = 0;
uint64_t count_y = 0;

int myclz(uint32_t x) {
  int r = 0, c;
  c = (x < 0x00010000) << 4;
  r += c;
  x <<= c;  // off 16
  c = (x < 0x01000000) << 3;
  r += c;
  x <<= c;  // off 8
  c = (x < 0x10000000) << 2;
  r += c;
  x <<= c;  // off 4
  c = (x < 0x40000000) << 1;
  r += c;
  x <<= c;  // off 2
  c = x < 0x80000000;
  r += c;
  x <<= c;  // off 1
  r += x == 0;
  return r;
}

bool isPerfectSquare(int num) {
  int clz = myclz(num);
  int shift = (31 - clz) >> 1;
  uint32_t x = (num >> shift);  // approximate square root
  while (x * x > num) {
    x = (x + num / x) / 2;
    count_x++;
  }
  uint64_t y = (num >> 1);  // divide by 2
  while (y * y > num) {
    y = (y + num / y) / 2;
    count_y++;
  }
  return x * x == num;
}

int main() {
  for (int i = 1; i < 100; i++) {
    bool result = isPerfectSquare(i);
  }

  printf("Optimized count: %lld\n", count_x);
  printf("Original  count: %lld\n", count_y);

  printf("count_y - count_x = %lld\n", count_y - count_x);
  printf("count_y / count_x = %f\n", (double)count_y / count_x);

  return 0;
}
