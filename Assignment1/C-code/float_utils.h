#ifndef FLOAT_UTILS_H
#define FLOAT_UTILS_H

#include <stdint.h> 

#define SIGN_MASK 0x80000000
#define NON_SIGN_MASK 0x7FFFFFFF
#define EXPONENT_MASK 0x7F800000
#define MANTISSA_MASK 0x007FFFFF
#define EXPONENT_BIAS 127
#define HIDDEN_BIT 0x00800000

#define SIGN_MASK_16 0x8000
#define EXPONENT_MASK_16 0x7F80
#define MANTISSA_MASK_16 0x007F
#define HIDDEN_BIT_16 0x0080

float fdiv2(float n);
float fsquare(float n);
float fadd(float a, float b);
float fdiv(float a, float b);
int fcmp(float a, float b);

typedef struct fp32 {
  uint32_t s;
  int32_t e;
  int32_t m;
} fp32;

fp32 unpack_fp32(uint32_t num);
uint32_t pack_fp32(fp32 fp);

typedef struct bf16 {
  uint32_t bits; // whole 16 bits
  uint32_t s; // sign
  int32_t e; // exponent
  int32_t m; // mantissa
} bf16;


bf16 fp32_bf16(uint32_t num);

bf16 bf16div2(bf16 n);


#endif