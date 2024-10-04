#include "float_utils.h"

#include <stdint.h>
#include <stdio.h>

#include "print_utils.h"

FloatingPoint unpack_float(uint32_t num) {
  struct FloatingPoint fp;

  fp.sign = (num & SIGN_MASK);
  fp.exponent = (num & EXPONENT_MASK) >> 23;
  fp.mantissa = num & MANTISSA_MASK | HIDDEN_BIT;

  return fp;
};

static inline int cbz32(uint32_t x) {
  int count = 0;
  for (int i = 0; i < 32; i++) {
    if (x & (1U << i)) break;
    count++;
  }
  return count;
}

static inline int cbz64(uint64_t x) {
  int count = 0;
  for (int i = 0; i < 64; i++) {
    if (x & (1LLU << i)) break;
    count++;
  }
  return count;
}

static inline int clz32(uint32_t x) {
  int count = 0;
  for (int i = 31; i >= 0; --i) {
    if (x & (1U << i)) break;
    count++;
  }
  return count;
}

static inline int clz64(uint64_t x) {
  int count = 0;
  for (int i = 63; i >= 0; --i) {
    if (x & (1LLU << i)) break;
    count++;
  }
  return count;
}

float fdiv2(float n) {
  uint32_t num = *(uint32_t*)&n;

  num -= 0x00800000;

  return *(float*)&num;
}

float fsquare(float n) {
  uint32_t num = *(uint32_t*)&n;

  FloatingPoint n_fp = unpack_float(num);
  n_fp.sign = 0;
  n_fp.exponent = (n_fp.exponent << 1) - EXPONENT_BIAS;
  uint64_t m = (uint64_t)n_fp.mantissa * n_fp.mantissa;
  int shift = 40 - clz64(m);  //
  int digit = 64 - clz64(m);
  int norm_shift = digit - 47;
  // Calculate the shift to normalize the mantissa. 47 is because the mantissa is a number with 23 numbers after the point. Therefore, if two mantissas are
  // multiplied, the result will have 46 numbers after points. If the result is normalized, there is 1 number before the point. Therefore, the shift is 47.
  // However, if there is 2 number before the point, the norm_shift is 48 - 47 = 1.
  n_fp.mantissa = (uint32_t)(m >> shift);
  n_fp.mantissa &= MANTISSA_MASK;
  n_fp.exponent += norm_shift;

  num = (n_fp.sign) | (n_fp.exponent << 23) | (n_fp.mantissa);

  return *(float*)&num;
}

// Function to compare two floating-point numbers
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

  FloatingPoint a_fp = unpack_float(ai);
  FloatingPoint b_fp = unpack_float(bi);

  uint32_t align = a_fp.exponent - b_fp.exponent > 24 ? 24 : a_fp.exponent - b_fp.exponent;
  b_fp.mantissa >>= align;

  if (a_fp.sign != b_fp.sign) {
    b_fp.mantissa = -b_fp.mantissa;  // negate b_fp.mantissa if signs are different
  }
  a_fp.mantissa += b_fp.mantissa;  // add mantissas

  uint32_t renorm_shift = clz32(a_fp.mantissa);
  if (renorm_shift > 8) {
    renorm_shift = renorm_shift - 8;
    a_fp.mantissa <<= renorm_shift;
    a_fp.exponent -= renorm_shift;
  } else {
    renorm_shift = 8 - renorm_shift;
    a_fp.mantissa >>= renorm_shift;
    a_fp.exponent += renorm_shift;
  }

  uint32_t result = a_fp.sign | (a_fp.exponent << 23) | (a_fp.mantissa & MANTISSA_MASK);

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

  FloatingPoint a_fp = unpack_float(ai);
  FloatingPoint b_fp = unpack_float(bi);

  a_fp.exponent -= b_fp.exponent;
  a_fp.exponent += EXPONENT_BIAS;

  a_fp.mantissa = idiv(a_fp.mantissa, b_fp.mantissa);
  int shift = !(a_fp.mantissa & 0x80000000);  // shift if the most significant bit is 0

  uint32_t result = a_fp.sign | ((a_fp.exponent - shift) << 23) | ((a_fp.mantissa >> 8 << shift) & MANTISSA_MASK);

  return *(float*)&result;
}
