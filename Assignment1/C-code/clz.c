#include <stdint.h>

#include "stdint.h"
#include "stdio.h"

void printBinary32(uint32_t n) {
  for (int i = 31; i >= 0; i--) {
    printf("%d", (n >> i) & 1);
    if (i % 4 == 0) {
      printf(" ");
    }
  }
  printf("\n");
}

uint32_t myclz(uint32_t x) {
  int r = !x, c;
  c = (x < 0x00010000) << 4;
  r += c;
  x <<= c;  // off 16
  c = (x < 0x01000000) << 3;
  r += c;
  x <<= c;  // off 8
  c = (x < 0x10000000) << 2;
  r += c;
  x <<= c;  // off 4
  c = (x >> (32 - 4 - 1)) & 0x1e;
  r += (0x55af >> c) & 3;
  return r;
}

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
  uint32_t x = 0x00001;
  uint32_t r = clz32(x);
  printf("r: %d\n", r);
  printBinary32(x);
  return 0;
}
