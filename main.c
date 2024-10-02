#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SIGN_MASK     0x80000000
#define EXPONENT_MASK 0x7F800000
#define MANTISSA_MASK 0x007FFFFF
#define EXPONENT_BIAS 127

// Function to print a 32-bit number in binary form
void printBinary(uint32_t num) {
    for (int i = 31; i >= 0; i--) {
       uint32_t mask = 1 << i;  // Create a mask to isolate each bit
       uint32_t bit = (num & mask) ? 1 : 0;  // Isolate the bit at position i
        printf("%u", bit);

        // Print a space after every 8 bits for readability
        if (i == 31 || i == 23) {
            printf(" ");
        }
    }
    printf("\n");
}

// fabs(guess * guess - n)
uint32_t squareDifference(uint32_t guess, uint32_t n) 
{
  /*
  guess^2
  */

  // Extract components of guess
  int32_t guess_exponent = (guess & EXPONENT_MASK) >> 23; // 8 bits for exponent
  uint32_t guess_mantissa = guess & MANTISSA_MASK | 0x00800000; // 23 bits for mantissa and add the implicit 1

  // Square the number by doubling the exponent and squaring the mantissa
  guess_exponent = (guess_exponent - EXPONENT_BIAS); // Double the exponent
  guess_exponent = ((guess_exponent - EXPONENT_BIAS) << 1) + EXPONENT_BIAS; // Double the exponent

  // Square the mantissa
  uint32_t squared_mantissa = ((uint64_t)guess_mantissa * guess_mantissa) >> 41; // Squaring the mantissa

  // Normalize the mantissa if necessary (it must fit into 23 bits)
  while (squared_mantissa > 0x00FFFFFF) 
  {
      squared_mantissa >>= 1;
      guess_exponent++;
  }

  // Reconstruct guess^2 in IEEE 754 format
  uint32_t squared_guess = (guess_exponent << 23) | (squared_mantissa & MANTISSA_MASK);

  /*
  guess^2 - n
  */

  // Extract exponent and mantissa of n
  uint32_t n_exponent = (n & EXPONENT_MASK) >> 23;
  uint32_t n_mantissa = (n & MANTISSA_MASK) | 0x00800000;

  // Align exponents by shifting the mantissa of the smaller exponent number
  if (guess_exponent > n_exponent) 
  {
    int shift = guess_exponent - n_exponent;
    n_mantissa >>= shift;
  } 
  else if (n_exponent > guess_exponent) 
  {
     int shift = n_exponent - guess_exponent;
    squared_mantissa >>= shift;
    guess_exponent = n_exponent; // Align to the higher exponent
  }
  
  // Subtract mantissas
 uint32_t result_mantissa = 0;
  if (squared_mantissa < n_mantissa) 
  {
    result_mantissa = n_mantissa - squared_mantissa;
  } 
  else 
  {
    result_mantissa = squared_mantissa - n_mantissa;
  }

  // Normalize the result mantissa if necessary
  while (result_mantissa < 0x00800000) 
  {
    result_mantissa <<= 1;
    guess_exponent--;
  }

  while (result_mantissa > 0x00FFFFFF) 
  {
    result_mantissa >>= 1;
    guess_exponent++;
  }

  // Reconstruct the result in IEEE 754 format
  uint32_t result = (guess_exponent << 23) | (result_mantissa & MANTISSA_MASK);

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
  } n = { .f = strtof(argv[1], &endptr) };

  printf("Number: %f\n", n.f);
  printf("Binary: ");
  printBinary(n.i);

  if (*endptr != '\0') {
    printf("Invalid input: %s\n", argv[1]);
    return 1;
  }

  /*
  int mySqrt(int n) {
    if (n == 0 || n == 1) {
        return n;  
    }

    double guess = n / 2.0;
    double e = 0.01; 
    while (fabs(guess * guess - n) > e) {
        guess = (guess + n / guess) / 2.0; 
    }
    return (int)guess;
  }
  */

  if (n.f == 1.0) {
    printf("%f is a valid perfect square.\n", n.f);
    return 0;
  }

  // Extracting sign, exponent and mantissa
  uint32_t sign     =  n.i & SIGN_MASK; // 1 bit
  int32_t exponent = (n.i & EXPONENT_MASK) >> 23; // 8 bits
  uint32_t mantissa =  n.i & MANTISSA_MASK; // 23 bits

  // dividing the exponent by 2
  exponent = ((exponent - EXPONENT_BIAS) >> 1) + EXPONENT_BIAS; // divide the exponent by 2

  union 
  {
    float f;
    uint32_t i;
  } guess = { .i = sign | (exponent << 23) | mantissa };

  printf("Initial guess: %f\n", guess.f);
  printBinary(guess.i);

  union 
  {
    float f;
    uint32_t i;
  } e = { .f = 0.01 };

  printf("guess^2 - n: %d\n", squareDifference(guess.i, n.i));

  // while (squareDifference(guess, n.f) > e.i) 
  // {
  //   guess = (guess + n.i / guess) / 2.0;
  // }



   
  return 0;
}