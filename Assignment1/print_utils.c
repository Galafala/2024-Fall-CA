#include "print_utils.h"

#include <stdio.h>  // For printf

// Function to print a 32-bit number in binary form (IEEE 754 format style)
void printBinary(uint32_t num) {
  for (int i = 31; i >= 0; i--) {
    uint32_t mask = 1 << i;               // Create a mask to isolate each bit
    uint32_t bit = (num & mask) ? 1 : 0;  // Isolate the bit at position i
    printf("%u", bit);

    // Print a space after the sign and exponent
    if (i == 31 || i == 23) {
      printf(" ");
    }
  }
  printf("\n\n");
}

// Function to print a 32-bit number in binary form (standard format)
void printBinary32(uint32_t num) {
  for (int i = 31; i >= 0; i--) {
    uint32_t mask = 1 << i;               // Create a mask to isolate each bit
    uint32_t bit = (num & mask) ? 1 : 0;  // Isolate the bit at position i
    printf("%u", bit);

    // Print a space after every 8 bits for readability
    if (i % 8 == 0 && i != 0) {
      printf(" ");
    }
  }
  printf("\n\n");
}

// Function to print a 64-bit number in binary form
void printBinary64(uint64_t num) {
  for (int i = 63; i >= 0; i--) {
    uint64_t mask = (uint64_t)1 << i;     // Create a mask to isolate each bit
    uint64_t bit = (num & mask) ? 1 : 0;  // Isolate the bit at position i
    printf("%llu", bit);

    // Print a space after every 8 bits for readability
    if (i % 8 == 0 && i != 0) {
      printf(" ");
    }
  }
  printf("\n\n");
}

// Function to print the float representation of a 32-bit number and its binary form
void printNumber(uint32_t num) {
  union {
    float f;
    uint32_t i;
  } n = {.i = num};

  printf("Number: %f\n", n.f);
  printf("Binary: ");
  printBinary(n.i);
}