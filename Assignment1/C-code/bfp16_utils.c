#include "bfp16_utils.h"

#include <stdint.h>

static inline int getbit(uint32_t num, int pos) { return (num >> (pos - 1)) & 1; }

static inline int clz16(uint16_t x) {
  int count = 0;
  for (int i = 15; i >= 0; --i) {
    if (x & (1 << i)) break;
    count++;
  }
  return count;
}

int32_t idiv(int32_t a, int32_t b) {
  uint32_t r = 0;
  for (int i = 0; i < 32; i++) {
    r <<= 1;
    if (a - b >= 0) {
      r |= 1;
      a -= b;
    }
    a <<= 1;
  }

  return r;
}

static inline uint32_t imul16(uint32_t a, uint32_t b) {
  uint32_t r = 0;
  for (int i = 0; i < 8; i++) {
    uint32_t mask = (b & 1) == 1 ? 0xFFFFFFFF : 0;
    r = (r >> 1) + (a & mask);
    b >>= 1;
  }

  return r;
}

/*
bf16: 16-bit bfloating-point number
*/

// s: 1 bit, e: 8 bits, m: 7 bits
// Since the numbers are all positive, we can handle it easily
// First, we don't need to handle the NaN case
bf16 fp32_to_bf16(uint32_t num) {
  int rounding = ((num >> 16) & 1) + 0x7FFF;  // The value rounds to the nearest even number
  num += rounding;
  num >>= 16;

  struct bf16 bfp;
  bfp.bits = num;
  bfp.s = 0;
  bfp.e = (num & EXPONENT_MASK_16) >> 7;
  bfp.m = (num & MANTISSA_MASK_16) | HIDDEN_BIT_16;

  return bfp;
};

float bf16_to_fp32(bf16 bfp) {
  uint32_t result = (bfp.bits << 16);

  return *(float*)&result;
}

/*
Arithmetic operations for bf16
*/

bf16 bf16div2(bf16 n) {
  n.e -= 1;
  n.bits = n.s | (n.e << 7) | (n.m & MANTISSA_MASK_16);
  return n;
}

bf16 bf16square(bf16 num) {
  num.s = 0;
  num.e = (num.e << 1) - EXPONENT_BIAS;
  uint32_t m = imul16(num.m, num.m);
  // If the 16th bit is 1, then the result needs to be normalized
  int norm_shift = getbit(m, 9);
  // Shift the m to the right by 7 bits. If the 16th bit is 1, shift by 8 bits.
  num.m = (m >> norm_shift) & MANTISSA_MASK_16;
  num.e += norm_shift;

  num.bits = num.s | (num.e << 7) | (num.m & MANTISSA_MASK_16);

  return num;
}

int bf16cmp(bf16 a, bf16 b) {
  if (a.bits == b.bits) {
    return 0;  // equal
  } else if (a.bits > b.bits) {
    return 1;  // greater
  }
  return 2;  // smaller
}

bf16 bf16add(bf16 a, bf16 b) {
  if ((a.bits & 0x7fff) < (b.bits & 0x7fff)) {
    bf16 tmp = a;
    a = b;
    b = tmp;
  }

  uint32_t align = a.e - b.e > 8 ? 8 : a.e - b.e;
  b.m >>= align;

  if (a.s != b.s) {
    b.m = -b.m;  // negate b_fp.m if signs are different
  }
  a.m += b.m;  // add mantissas

  uint32_t renorm_shift = clz16(a.m);
  if (renorm_shift > 8) {
    renorm_shift = renorm_shift - 8;
    a.m <<= renorm_shift;
    a.e -= renorm_shift;
  } else {
    renorm_shift = 8 - renorm_shift;
    a.m >>= renorm_shift;
    a.e += renorm_shift;
  }

  a.bits = a.s | (a.e << 7) | (a.m & MANTISSA_MASK_16);

  return a;
}

bf16 bf16div(bf16 a, bf16 b) {
  a.e -= b.e;
  a.e += EXPONENT_BIAS;

  a.m = idiv(a.m, b.m);
  int shift = !(a.m & 0x8000);  // shift if the most significant bit is 0

  a.bits = a.s | ((a.e - shift) << 7) | ((a.m >> 1 << shift) & MANTISSA_MASK_16);

  return a;
}