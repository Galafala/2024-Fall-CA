#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "float_arithmetic.h"
#include "print_utils.h"

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
  uint32_t squared_guess = multiplyFloat(guess, guess);
  uint32_t result = subtractFloat(squared_guess, n);
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

  union {
    float f;
    uint32_t i;
  } error = {.f = 0.0001};

  while (compare(squareDifference(guess, n.i), error.i)) {
    // guess = (guess + n / guess) / 2.0
    uint32_t temp = divisionFloat(n.i, guess);
    guess = addFloat(guess, temp);
    guess -= 0x00800000;
  }

  printf("Square root:\n");
  printNumber(guess);

  return 0;
}