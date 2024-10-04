#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include <stdint.h>  // For uint32_t and uint64_t types

// Function to print a 32-bit number in binary form (IEEE 754 format style)
void printBinary(uint32_t num);

// Function to print a 32-bit number in binary form (standard format)
void printBinary32(uint32_t num);

// Function to print a 64-bit number in binary form
void printBinary64(uint64_t num);

// Function to print the float representation of a 32-bit number and its binary form
void printNumber(uint32_t num);

void fprint(float num);

#endif  // PRINT_UTILS_H