#include <stdio.h>

int main(void) {
  int i = 33;

  for (i; i < 128; i++) {
    printf("%d\t%c \n", i, i);
  }

  return 0;
}