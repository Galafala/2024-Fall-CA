#ifndef BFP16_UTILS_H
#define BFP16_UTILS_H

#include <stdint.h> 

#define SIGN_MASK_16 0x8000
#define EXPONENT_MASK_16 0x7F80
#define MANTISSA_MASK_16 0x007F
#define HIDDEN_BIT_16 0x0080
#define EXPONENT_BIAS 127

typedef struct bf16 {
  uint32_t bits; // whole 16 bits
  uint32_t s; // sign
  int32_t e; // exponent
  int32_t m; // mantissa
} bf16;

bf16 fp32_to_bf16(uint32_t num);
float bf16_to_fp32(bf16 bfp);
int bf16cmp(bf16 a, bf16 b);
bf16 bf16add(bf16 a, bf16 b);
bf16 bf16square(bf16 n);
bf16 bf16div(bf16 a, bf16 b);
bf16 bf16div2(bf16 n);

#endif