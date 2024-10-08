#include "float_utils.h"

#include <stdint.h>
#include <stdio.h>

#include "print_utils.h"

fp32 unpack_fp32(uint32_t num) {
  struct fp32 fp;

  fp.s = (num & SIGN_MASK);
  fp.e = (num & EXPONENT_MASK) >> 23;
  fp.m = (num & MANTISSA_MASK) | HIDDEN_BIT;

  return fp;
};

uint32_t pack_fp32(fp32 fp) {
  uint32_t num = (fp.s) | (fp.e << 23) | (fp.m & MANTISSA_MASK);
  return num;
};

static inline int clz32(uint32_t x) {
  int count = 0;
  for (int i = 31; i >= 0; --i) {
    if (x & (1U << i)) break;
    count++;
  }
  return count;
}

float fdiv2(float n) {
  uint32_t num = *(uint32_t*)&n;

  num -= 0x00800000;

  return *(float*)&num;
}

static inline int getbit(uint32_t num, int pos) { return (num >> (pos - 1)) & 1; }

uint32_t imul32(uint32_t a, uint32_t b) {
  uint32_t r = 0;
  for (int i = 0; i < 24; i++) {
    uint32_t mask = (b & 1) == 1 ? 0xFFFFFFFF : 0;
    r = (r >> 1) + (a & mask);
    b >>= 1;
  }

  return r;
}

float fsquare(float n) {
  uint32_t num = *(uint32_t*)&n;

  fp32 n_fp = unpack_fp32(num);
  n_fp.s = 0;
  n_fp.e = (n_fp.e << 1) - EXPONENT_BIAS;
  uint32_t m = imul32(n_fp.m, n_fp.m);
  // If the 48th bit is 1, then the result needs to be normalized
  int norm_shift = getbit(m, 25);
  // Shift the m to the right by 23 bits. If the 48th bit is 1, shift by 24 bits.
  n_fp.m = (m >> norm_shift) & MANTISSA_MASK;
  n_fp.e += norm_shift;

  num = (n_fp.s) | (n_fp.e << 23) | (n_fp.m);

  return *(float*)&num;
}

// Function to compare two floating-point numbers, and only need to handle positive numbers
int fcmp(float a, float b) {
  uint32_t ai = *(uint32_t*)&a;
  uint32_t bi = *(uint32_t*)&b;

  if (ai == bi) {
    return 0;  // equal
  } else if (ai > bi) {
    return 1;  // greater
  }
  return 2;  // smaller
}

float fadd(float a, float b) {
  uint32_t ai = *(uint32_t*)&a;
  uint32_t bi = *(uint32_t*)&b;

  if ((ai & NON_SIGN_MASK) < (bi & NON_SIGN_MASK)) {
    uint32_t tmp = ai;
    ai = bi;
    bi = tmp;
  }

  fp32 a_fp = unpack_fp32(ai);
  fp32 b_fp = unpack_fp32(bi);

  uint32_t align = a_fp.e - b_fp.e > 24 ? 24 : a_fp.e - b_fp.e;
  b_fp.m >>= align;

  if (a_fp.s != b_fp.s) {
    b_fp.m = -b_fp.m;  // negate b_fp.m if signs are different
  }
  a_fp.m += b_fp.m;  // add mantissas

  uint32_t renorm_shift = clz32(a_fp.m);
  if (renorm_shift > 8) {
    renorm_shift = renorm_shift - 8;
    a_fp.m <<= renorm_shift;
    a_fp.e -= renorm_shift;
  } else {
    renorm_shift = 8 - renorm_shift;
    a_fp.m >>= renorm_shift;
    a_fp.e += renorm_shift;
  }

  uint32_t result = a_fp.s | (a_fp.e << 23) | (a_fp.m & MANTISSA_MASK);

  return *(float*)&result;
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

float fdiv(float a, float b) {
  uint32_t ai = *(uint32_t*)&a;
  uint32_t bi = *(uint32_t*)&b;

  fp32 a_fp = unpack_fp32(ai);
  fp32 b_fp = unpack_fp32(bi);

  a_fp.e -= b_fp.e;
  a_fp.e += EXPONENT_BIAS;

  a_fp.m = idiv(a_fp.m, b_fp.m);
  int shift = !(a_fp.m & 0x80000000);  // shift if the most significant bit is 0

  uint32_t result = a_fp.s | ((a_fp.e - shift) << 23) | ((a_fp.m >> 8 << shift) & MANTISSA_MASK);

  return *(float*)&result;
}

/*
bf16: 16-bit brain floating-point number
*/

// s: 1 bit, e: 8 bits, m: 7 bits
// Since the numbers are all positive, we can handle it easily
// First, we don't need to handle the NaN case
bf16 fp32_bf16(uint32_t num) {
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

bf16 bf16div2(bf16 n) {
  n.e -= 1;
  return n;
}

static inline int clz16(uint16_t x) {
  int count = 0;
  for (int i = 15; i >= 0; --i) {
    if (x & (1 << i)) break;
    count++;
  }
  return count;
}

uint32_t imul16(uint32_t a, uint32_t b) {
  uint32_t r = 0;
  for (int i = 0; i < 8; i++) {
    uint32_t mask = (b & 1) == 1 ? 0xFFFFFFFF : 0;
    r = (r >> 1) + (a & mask);
    b >>= 1;
  }

  return r;
}

bf16 bf16square(bf16 bfp) {
  bfp.s = 0;
  bfp.e = (bfp.e << 1) - EXPONENT_BIAS;
  uint32_t m = imul16(bfp.m, bfp.m);
  // If the 16th bit is 1, then the result needs to be normalized
  int norm_shift = getbit(m, 9);
  // Shift the m to the right by 7 bits. If the 16th bit is 1, shift by 8 bits.
  bfp.m = (m >> norm_shift) & MANTISSA_MASK_16;
  bfp.e += norm_shift;

  return bfp;
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

  // The mantissas are positive, so we don't need to handle the sign bit
  // if (a.s != b.s) {
  //   b.m = -b.m;  // negate b_fp.m if signs are different
  // }
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