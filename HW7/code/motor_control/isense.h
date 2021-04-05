#ifndef isense__H__
#define isense__H__


// Initialize the ADC module.
void ADC_init();

// Convert ADC
unsigned int convert_adc();

// Reads the ADC and returns the value, 0-1023
unsigned int read_adc();

// Gets the current using the liner correlation between ADC counts to current
int read_current();

#endif