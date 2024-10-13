#include "stdint.h"
#include "stdio.h"

int clz32(uint32_t x) {
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

int main() {
  int test[6] = {0, 0x345, 0x80000000, 0x1, 0xc2440000, 0xf00001};

  for (int i = 0; i < 6; i++) {
    int r = clz32(test[i]);
    if (r == 32) {
      printf("The number of %d's leading zero is udefined\n", i);
      continue;
    }
    printf("The number of %d's leading zero is %d\n", i, r);
  }

  return 0;
}