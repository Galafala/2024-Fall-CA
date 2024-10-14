#include "fp32_utils.h"

#include <stdint.h>
#include <stdio.h>

#include "print_utils.h"

static inline int getbit32(uint32_t num, int pos) { return (num >> (pos - 1)) & 1; }

fp32 unpack_float(uint32_t num) {
  struct fp32 fp;

  fp.sign = (num & SIGN_MASK);
  fp.exponent = (num & EXPONENT_MASK) >> 23;
  fp.mantissa = num & MANTISSA_MASK | HIDDEN_BIT;

  return fp;
};

static inline int clz32(uint32_t x) {
  int count = 0;
  for (int i = 31; i >= 0; --i) {
    if (x & (1U << i)) break;
    count++;
  }
  return count;
}

static inline int cbz32(uint32_t x) {
  int count = 0;
  for (int i = 0; i < 32; ++i) {
    if (x & (1U << i)) break;
    count++;
  }
  return count;
}

uint32_t fp32_to_uint32(float n) {
  uint32_t num = *(uint32_t *)&n;
  fp32 fp = unpack_float(num);

  fp.exponent -= EXPONENT_BIAS;
  int shift = 23 - fp.exponent;
  if (shift > 0) {
    fp.mantissa = fp.mantissa + (getbit32(fp.mantissa, shift) << shift);  // rounding 24-3=21
    fp.mantissa >>= shift;
  } else {
    fp.mantissa <<= -shift;
  }

  return fp.mantissa;
}

// Function to compare two floating-point numbers, and only need to handle positive numbers
int fcmp(float a, float b) {
  uint32_t ai = *(uint32_t *)&a;
  uint32_t bi = *(uint32_t *)&b;

  if (ai == bi) {
    return 0;  // equal
  } else if (ai > bi) {
    return 1;  // greater
  }
  return 2;  // smaller
}

float fabsf(float x) {
  uint32_t i = *(uint32_t *)&x;  // Read the bits of the float into an integer
  i &= 0x7FFFFFFF;               // Clear the sign bit to get the absolute value
  x = *(float *)&i;              // Write the modified bits back into the float
  return x;
}

float fnegative(float x) {
  uint32_t i = *(uint32_t *)&x;  // Read the bits of the float into an integer
  i ^= 0x80000000;               // Toggle the sign bit to get the negative value
  x = *(float *)&i;              // Write the modified bits back into the float
  return x;
}

float fdiv2(float n) {
  uint32_t num = *(uint32_t *)&n;

  num -= 0x00800000;

  return *(float *)&num;
}

uint32_t imul(uint32_t a, uint32_t b) {
  uint32_t r = 0;
  for (int i = 0; i < 24; i++) {
    uint32_t mask = (b & 1) == 1 ? 0xFFFFFFFF : 0;
    r = (r >> 1) + (a & mask);
    b >>= 1;
  }

  return r;
}

float fsquare(float n) {
  uint32_t num = *(uint32_t *)&n;

  fp32 n_fp = unpack_float(num);
  n_fp.sign = 0;
  n_fp.exponent = (n_fp.exponent << 1) - EXPONENT_BIAS;
  uint32_t m = imul(n_fp.mantissa, n_fp.mantissa);
  // If the 48th bit is 1, then the result needs to be normalized
  int norm_shift = getbit32(m, 25);
  // Shift the mantissa to the right by 23 bits. If the 48th bit is 1, shift by 24 bits.
  n_fp.mantissa = (m >> norm_shift) & MANTISSA_MASK;
  n_fp.exponent += norm_shift;

  num = (n_fp.sign) | (n_fp.exponent << 23) | (n_fp.mantissa);

  return *(float *)&num;
}

float fadd(float a, float b) {
  uint32_t ai = *(uint32_t *)&a;
  uint32_t bi = *(uint32_t *)&b;

  if ((ai & NON_SIGN_MASK) < (bi & NON_SIGN_MASK)) {
    uint32_t tmp = ai;
    ai = bi;
    bi = tmp;
  }

  fp32 a_fp = unpack_float(ai);
  fp32 b_fp = unpack_float(bi);

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

  return *(float *)&result;
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
  uint32_t ai = *(uint32_t *)&a;
  uint32_t bi = *(uint32_t *)&b;

  fp32 a_fp = unpack_float(ai);
  fp32 b_fp = unpack_float(bi);

  a_fp.exponent -= b_fp.exponent;
  a_fp.exponent += EXPONENT_BIAS;

  a_fp.mantissa = idiv(a_fp.mantissa, b_fp.mantissa);
  int shift = !(a_fp.mantissa & 0x80000000);  // shift if the most significant bit is 0

  uint32_t result = a_fp.sign | ((a_fp.exponent - shift) << 23) | ((a_fp.mantissa >> 8 << shift) & MANTISSA_MASK);

  return *(float *)&result;
}
