#include "NU32.h"          // constants, funcs for startup and UART


// Begins sampling from a specifed ANx pin, waits for the conversion to
// complete, and returns the result
unsigned int adc_sample_convert(int pin) {  // sample & convert the value on the given
                                            // adc pin the pin should be configured as an
                                            // analog input in AD1PCFG
    unsigned int elapsed = 0, finish_time = 0;

    AD1CON1bits.ON = 1;                     // ADC enabled
    AD1CON1bits.SSRC = 0b111;               // Auto Conversion
    AD1CON1bits.ASAM = 0;  

    AD1CHSbits.CH0SA = pin;                 // connect chosen pin to MUXA for sampling
    AD1CON1bits.SAMP = 1;                   // start sampling
    elapsed = _CP0_GET_COUNT();
    finish_time = elapsed + SAMPLE_TIME;
    while (_CP0_GET_COUNT() < finish_time) {
    ;                                       // sample for more than 250 ns
    }
    AD1CON1bits.SAMP = 0;                   // stop sampling and start converting
    while (!AD1CON1bits.DONE) {
    ;                                       // wait for the conversion process to finish
    }
    return ADC1BUF0;                        // read the buffer with the result
}