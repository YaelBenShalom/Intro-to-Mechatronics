#include "helper.h"

void calculateGrowth(Investment *invp) {
  int i;

  for (i=1; i<=invp->years; i= i+1) {
    invp->invarray[i] = invp->growth * invp->invarray[i-1];
  }
}

int getUserInput(Investment *invp) {
  int valid;
  char statement[100], value[100], validStatement[100], invalidStatement[100];

  sprintf(statement,"Enter investment, growth rate, number of yrs (up to %d): ", MAX_YEARS);
  NU32_WriteUART3(statement);
  NU32_ReadUART3(value, 100);
  sscanf(value,"%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));

  valid = (invp->inv0 > 0) && (invp->growth > 0) &&
          (invp->years > 0) && (invp->years <= MAX_YEARS);
  sprintf(validStatement,"Valid input?  %d\n",valid);
  NU32_WriteUART3(validStatement);

  if (!valid) {
    sprintf(invalidStatement,"Invalid input; exiting.\n");
    NU32_WriteUART3(invalidStatement);
  }
  return(valid);
}

void sendOutput(double *arr, int yrs) {
  int i;
  char outstring[100], resultStatement[100], *newLine;

  sprintf(resultStatement,"\nRESULTS:\n\n");
  NU32_WriteUART3(resultStatement);
  for (i=0; i<=yrs; i++) {
    sprintf(outstring,"Year %3d:  %10.2f\n",i,arr[i]);
    NU32_WriteUART3(outstring);
  }
  sprintf(newLine,"\n");
  NU32_WriteUART3(newLine);
}