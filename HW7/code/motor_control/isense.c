#include "NU32.h"                           // config bits, constants, funcs for startup and UART
#include "isense.h"

#include <stdio.h>
#include <xc.h>


void ADC_init(void) {
  AD1CON1bits.SSRC = 0x7;                   //Set the conversion process to be automatic
  AD1CON1bits.ASAM = 0;                     //Set the sampling process to be manual
  AD1CON3bits.ADCS = 2;                     //Set Tad to be 2*(ADCS+1)*Tpb = 75ns
  AD1CON3bits.SAMC = 2;                     //Set sampling time to be 2*Tad = 150ns
  AD1CON1bits.ADON = 1;                     // turn on A/D converter
}

unsigned int convert_adc(int pin) {         // sample & convert the value on the given

  unsigned int elapsed_time = 0;
  unsigned int finish_time = 0;

  AD1CHSbits.CH0SA = pin;                   // connect chosen pin to MUXA for sampling
  AD1CON1bits.SAMP = 1;                     // start sampling
  elapsed_time = _CP0_GET_COUNT();
  finish_time = elapsed_time + 10;

  while (_CP0_GET_COUNT() < finish_time) {;}  // sample for more than 250 ns

  AD1CON1bits.SAMP = 0;                     // stop sampling and start converting

  while (!AD1CON1bits.DONE) {;}             // wait for the conversion process to finish

  return ADC1BUF0;                          // read the buffer with the result
}

unsigned int read_adc() {
  int i = 0;
  int sum = 0;

  while (i<=10) {
    sum += convert_adc(16);
    i++;
  }
  return (sum/10);
}

int read_current(void) {
  int adc_count = read_adc();
  float adc_mA = 5.702 * adc_count - 3199;
  return adc_mA;
}