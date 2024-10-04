#include "float_utils.h"

floatingPoint unpackFloat(uint32_t num) {
  struct floatingPoint fp;

  fp.sign = (num & SIGN_MASK);
  fp.exponent = (num & EXPONENT_MASK) >> 23;
  fp.mantissa = num & MANTISSA_MASK | HIDDEN_BIT;

  return fp;
};

uint32_t addFloat(uint32_t a, uint32_t b) {
  floatingPoint a_fp = unpackFloat(a);
  floatingPoint b_fp = unpackFloat(b);

  // Align exponents by shifting the mantissa of the smaller exponent number

  if (a_fp.exponent > b_fp.exponent) {
    int shift = a_fp.exponent - b_fp.exponent;
    b_fp.mantissa >>= shift;
    b_fp.exponent = a_fp.exponent;  // Align to the higher exponent
  } else if (b_fp.exponent > a_fp.exponent) {
    int shift = b_fp.exponent - a_fp.exponent;
    a_fp.mantissa >>= shift;
    a_fp.exponent = b_fp.exponent;  // Align to the higher exponent
  }

  // Add mantissas
  uint32_t result_mantissa = a_fp.mantissa + b_fp.mantissa;

  // Normalize the result mantissa if necessary
  while (result_mantissa < 0x00800000 && result_mantissa > 0) {
    result_mantissa <<= 1;
    a_fp.exponent--;
  }

  while (result_mantissa > 0x00FFFFFF) {
    result_mantissa >>= 1;
    a_fp.exponent++;
  }

  // Reconstruct the result in IEEE 754 format
  uint32_t result = (a_fp.sign) | (a_fp.exponent << 23) | (result_mantissa & MANTISSA_MASK);

  return result;
}

uint32_t divisionFloat(uint32_t a, uint32_t b) {
  floatingPoint a_fp = unpackFloat(a);
  floatingPoint b_fp = unpackFloat(b);

  // Step 1: Divide mantissas (mantissa_a / mantissa_b)
  uint64_t result_mantissa = ((uint64_t)a_fp.mantissa << 23) / b_fp.mantissa;

  // Step 2: Subtract exponents (exponent_a - exponent_b + bias)
  int32_t result_exponent = (a_fp.exponent - b_fp.exponent) + EXPONENT_BIAS;

  // Step 3: Normalize the result mantissa if necessary
  if (result_mantissa & HIDDEN_BIT) {
    // If the hidden bit is set, we are already normalized
  } else {
    // Normalize: shift mantissa left until the hidden bit is restored
    while ((result_mantissa & HIDDEN_BIT) == 0 && result_exponent > 0) {
      result_mantissa <<= 1;
      result_exponent--;
    }
  }

  // Step 4: Handle edge case for denormalized numbers
  if (result_exponent <= 0) {
    // Denormalized result, shift mantissa to the right to fit exponent = 0
    result_mantissa >>= (1 - result_exponent);
    result_exponent = 0;
  }

  // Step 5: Handle overflow in exponent (e.g., result too large)
  if (result_exponent >= 255) {
    // Overflow: return infinity
    return (a_fp.sign) | EXPONENT_MASK;
  }

  // Step 6: Reconstruct the result (sign, exponent, mantissa)
  result_mantissa &= MANTISSA_MASK;  // Remove the hidden bit
  uint32_t result = (a_fp.sign) | (result_exponent << 23) | result_mantissa;

  return result;
}

uint32_t fmul(uint32_t a, uint32_t b) {
  floatingPoint a_fp = unpackFloat(a);
  floatingPoint b_fp = unpackFloat(b);

  // Multiply mantissas (mantissa_a * mantissa_b)
  uint32_t result_mantissa = ((uint64_t)a_fp.mantissa * b_fp.mantissa) >> 23;

  // Add exponents (exponent_a + exponent_b - bias)
  int32_t result_exponent = (a_fp.exponent + b_fp.exponent) - EXPONENT_BIAS;

  // Normalize the result mantissa if necessary
  while (result_mantissa > 0x00FFFFFF) {
    result_mantissa >>= 1;
    result_exponent++;
  }

  // Handle overflow in exponent (e.g., result too large)
  if (result_exponent >= 255) {
    // Overflow: return infinity
    return (a_fp.sign) | EXPONENT_MASK;
  }

  // Reconstruct the result (sign, exponent, mantissa)
  result_mantissa &= MANTISSA_MASK;  // Remove the hidden bit

  uint32_t result = (a_fp.sign) | (result_exponent << 23) | result_mantissa;

  return result;
}

// Function to subtract two floating-point numbers in IEEE 754 format, returning the absolute result
uint32_t subtractFloat(uint32_t a, uint32_t b) {
  floatingPoint a_fp = unpackFloat(a);
  floatingPoint b_fp = unpackFloat(b);

  if (a == b) {
    return 0;
  }

  // Align exponents by shifting the mantissa of the smaller exponent number
  if (a_fp.exponent > b_fp.exponent) {
    int shift = a_fp.exponent - b_fp.exponent;
    b_fp.mantissa >>= shift;
    b_fp.exponent = a_fp.exponent;  // Align to the higher exponent
  } else if (b_fp.exponent > a_fp.exponent) {
    int shift = b_fp.exponent - a_fp.exponent;
    a_fp.mantissa >>= shift;
    a_fp.exponent = b_fp.exponent;  // Align to the higher exponent
  }

  // Ensure we subtract the smaller mantissa from the larger mantissa
  uint32_t result_mantissa;
  if (a_fp.mantissa >= b_fp.mantissa) {
    result_mantissa = a_fp.mantissa - b_fp.mantissa;
  } else {
    result_mantissa = b_fp.mantissa - a_fp.mantissa;
  }

  // Normalize the result mantissa if necessary
  if (result_mantissa != 0) {  // Check to avoid normalization on zero
    while (result_mantissa < HIDDEN_BIT && a_fp.exponent > 0) {
      result_mantissa <<= 1;
      a_fp.exponent--;
    }
  }

  // Handle potential overflow (if result mantissa exceeds 23 bits)
  while (result_mantissa > 0x00FFFFFF) {
    result_mantissa >>= 1;
    a_fp.exponent++;
  }

  // If exponent becomes zero, the number is denormalized
  if (a_fp.exponent == 0) {
    result_mantissa >>= 1;
  }

  // Remove the hidden leading 1 from the mantissa for normalized numbers
  result_mantissa &= MANTISSA_MASK;

  // Reconstruct the result in IEEE 754 format
  uint32_t result = (a_fp.sign) | (a_fp.exponent << 23) | result_mantissa;

  return result;
}

float fdiv2(float n) {
  union {
    float f;
    uint32_t i;
  } n_u = {.f = n};

  n_u.i -= 0x00800000;

  return n_u.f;
}