#ifndef FLOAT_UTILS_H
#define FLOAT_UTILS_H

#include <stdint.h>  // For uint32_t and int32_t types

#define SIGN_MASK 0x80000000
#define NON_SIGN_MASK 0x7FFFFFFF
#define EXPONENT_MASK 0x7F800000
#define MANTISSA_MASK 0x007FFFFF
#define EXPONENT_BIAS 127
#define HIDDEN_BIT 0x00800000

float fadd(float a, float b);
float fmul(float a, float b);
float fdiv(float a, float b);
float fdiv2(float n);
float fsquare(float n);
int32_t idiv(int32_t a, int32_t b);
int fcmp(float a, float b);

typedef struct FloatingPoint {
  uint32_t sign;
  int32_t exponent;
  int32_t mantissa;
} FloatingPoint;

FloatingPoint unpack_float(uint32_t num);

#endif  // FLOAT_ARITHMETIC_H