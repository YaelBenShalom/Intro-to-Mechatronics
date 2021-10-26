#ifndef io__H__
#define io__H__

// Global variables:
#define MAX_YEARS 100

// Investment structure:
typedef struct {
  double inv0;
  double growth;
  int years;
  double invarray[MAX_YEARS + 1];
} Investment;

// Function prototypes:
int getUserInput(Investment *invp);
void sendOutput(double *arr, int years);

#endif