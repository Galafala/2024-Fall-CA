#include "stdint.h"
#include "stdio.h"

static inline int clz32(uint32_t x) {
  int count = 0;
  for (int i = 31; i >= 0; --i) {
    if (x & (1U << i)) break;
    count++;
  }
  return count;
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