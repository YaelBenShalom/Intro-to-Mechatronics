#include <stdio.h>

// Function for char operations
void char_operations(char c1, char c2, char c3) {
  char outstring[100];

  c3 = c1 + c2;
  c3 = c1 - c2;
  c3 = c1 * c2;
  c3 = c1 / c2;
}

// Function for int operations
void int_operations(int i1, int i2, int i3) {
  char outstring[100];

  i3 = i1 + i2;
  i3 = i1 - i2;
  i3 = i1 * i2;
  i3 = i1 / i2;
}

// Function for long long int operations
void long_long_int_operations(long long int j1, long long int j2,
                              long long int j3) {
  char outstring[100];

  j3 = j1 + j2;
  j3 = j1 - j2;
  j3 = j1 * j2;
  j3 = j1 / j2;
}

// Function for float operations
void float_operations(float f1, float f2, float f3) {
  char outstring[100];

  f3 = f1 + f2;
  f3 = f1 - f2;
  f3 = f1 * f2;
  f3 = f1 / f2;
}

// Function for long double operations
void long_double_operations(long double d1, long double d2, long double d3) {
  char outstring[100];

  d3 = d1 + d2;
  d3 = d1 - d2;
  d3 = d1 * d2;
  d3 = d1 / d2;
}

// Main function
void main(void) {

  char c1 = 5, c2 = 6, c3;
  int i1 = 5, i2 = 6, i3;
  long long int j1 = 5, j2 = 6, j3;
  float f1 = 1.01, f2 = 2.02, f3;
  long double d1 = 1.01, d2 = 2.02, d3;

  char_operations(c1, c2, c3);
  int_operations(i1, i2, i3);
  long_long_int_operations(j1, j2, j3);
  float_operations(f1, f2, f3);
  long_double_operations(d1, d2, d3);
}