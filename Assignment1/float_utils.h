#ifndef FLOAT_UTILS_H
#define FLOAT_UTILS_H

#include <stdint.h>  // For uint32_t and int32_t types

#define SIGN_MASK 0x80000000
#define EXPONENT_MASK 0x7F800000
#define MANTISSA_MASK 0x007FFFFF
#define EXPONENT_BIAS 127
#define HIDDEN_BIT 0x00800000

uint32_t addFloat(uint32_t a, uint32_t b);
uint32_t subtractFloat(uint32_t a, uint32_t b);
uint32_t fmul(uint32_t a, uint32_t b);
uint32_t divisionFloat(uint32_t a, uint32_t b);
float fdiv2(float n);

typedef struct floatingPoint {
  uint32_t sign;
  int32_t exponent;
  uint32_t mantissa;
} floatingPoint;

floatingPoint unpackFloat(uint32_t num);

#endif  // FLOAT_ARITHMETIC_H