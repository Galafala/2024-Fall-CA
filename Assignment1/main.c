#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "print_utils.h"

#define SIGN_MASK 0x80000000
#define EXPONENT_MASK 0x7F800000
#define MANTISSA_MASK 0x007FFFFF
#define EXPONENT_BIAS 127
#define HIDDEN_BIT 0x00800000

typedef struct floatingPoint {
  uint32_t sign;
  int32_t exponent;
  uint32_t mantissa;
} floatingPoint;

floatingPoint unpackFloat(uint32_t num) {
  struct floatingPoint fp;

  fp.sign = (num & SIGN_MASK);
  fp.exponent = (num & EXPONENT_MASK) >> 23;
  fp.mantissa = num & MANTISSA_MASK | HIDDEN_BIT;

  return fp;
};

uint32_t multiplyAbsFloat(uint32_t a, uint32_t b) {
  floatingPoint a_fp = unpackFloat(a);
  floatingPoint b_fp = unpackFloat(b);

  // Multiply mantissas (mantissa_a * mantissa_b)
  uint64_t result_mantissa = ((uint64_t)a_fp.mantissa * b_fp.mantissa) >> 23;

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
uint32_t substractAbsFloat(uint32_t a, uint32_t b) {
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

uint32_t divideByTwo(uint32_t num) {
  // Extract components of num
  int32_t exponent = (num & EXPONENT_MASK) >> 23;        // 8 bits for exponent
  uint32_t mantissa = num & MANTISSA_MASK | HIDDEN_BIT;  // 23 bits for mantissa and add the implicit 1

  // Divide the number by halving the exponent
  exponent--;

  // Normalize the mantissa if necessary (it must fit into 23 bits)
  while (mantissa > 0x00FFFFFF) {
    mantissa >>= 1;
    exponent++;
  }

  // Reconstruct the number in IEEE 754 format
  uint32_t result = (exponent << 23) | (mantissa & MANTISSA_MASK);

  return result;
}

// fabs(guess * guess - n)
uint32_t squareDifference(uint32_t guess, uint32_t n) {
  // Reconstruct guess^2 in IEEE 754 format
  uint32_t squared_guess = multiplyAbsFloat(guess, guess);
  uint32_t result = substractAbsFloat(squared_guess, n);
  return result;
}

// Function to compare two floating-point numbers
bool compare(uint32_t a, uint32_t b) {
  floatingPoint a_fp = unpackFloat(a);
  floatingPoint b_fp = unpackFloat(b);

  // Step 1: Compare signs
  if (a_fp.sign != b_fp.sign) {
    // If the signs are different, positive is always greater than negative
    return a_fp.sign < b_fp.sign;
  }

  // Step 2: Compare exponents (after sign comparison)
  if (a_fp.exponent > b_fp.exponent) {
    return a_fp.sign == 0;  // If both are positive, greater exponent wins
  } else if (a_fp.exponent < b_fp.exponent) {
    return a_fp.sign != 0;  // If both are negative, smaller exponent wins
  }

  // Step 3: Compare mantissas (if signs and exponents are the same)
  if (a_fp.mantissa > b_fp.mantissa) {
    return a_fp.sign == 0;  // For positive numbers, larger mantissa is greater
  } else if (a_fp.mantissa < b_fp.mantissa) {
    return a_fp.sign != 0;  // For negative numbers, smaller mantissa is greater
  }

  // If everything is the same, they are equal
  return false;
}

uint32_t floatAbsAdd(uint32_t a, uint32_t b) {
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

uint32_t divisionAbsFloat(uint32_t a, uint32_t b) {
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

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <number>\n", argv[0]);
    return 1;
  }

  char *endptr;

  union {
    float f;
    uint32_t i;
  } n = {.f = strtof(argv[1], &endptr)};

  if (*endptr != '\0') {
    printf("Invalid input: %s\n", argv[1]);
    return 1;
  }

  printf("Input:\n");
  printNumber(n.i);

  if (n.f == 1.0 || n.f == 0.0) {
    printf("%f is a valid perfect square.\n", n.f);
    return 0;
  }

  // guess = n / 2.0
  uint32_t guess = n.i - 0x00800000;

  printf("guess: \n");
  printNumber(guess);

  union {
    float f;
    uint32_t i;
  } error = {.f = 0.0001};

  while (compare(squareDifference(guess, n.i), error.i)) {
    // guess = (guess + n / guess) / 2.0
    uint32_t temp = divisionAbsFloat(n.i, guess);
    guess = floatAbsAdd(guess, temp);
    guess = divideByTwo(guess);
  }

  printf("Square root:\n");
  printNumber(guess);

  return 0;
}