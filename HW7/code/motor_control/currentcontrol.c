#include "currentcontrol.h"
#include "NU32.h" // config bits, constants, funcs for startup and UART
#include "encoder.h"
#include "isense.h"
#include "utilities.h"

#include <stdio.h>
#include <xc.h>

static volatile int counter = 0; // initialize counter once
static volatile int I;
static volatile int e = 0;
static volatile int e_int = 0;
static volatile int StoringData =
    0; // if this flag = 1, currently storing plot data

static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int I_array[NUMSAMPS];  // measured values to plot
static volatile int REFarray[NUMSAMPS]; // reference values to plot

void PWM_init() {

  // Timer 2 - 5kHz
  T2CONbits.TCKPS = 0;
  PR2 = 15999;
  TMR2 = 0;
  T2CONbits.ON = 1;
  IPC2bits.T2IP = 3; // step 4: interrupt priority 3
  IPC2bits.T2IS = 0;
  IFS0bits.T2IF = 0; // step 5: clear the int flag
  IEC0bits.T2IE = 1; // step 6: enable INT0 by setting IEC0<3>

  // Timer 3 - 20kHz
  T3CONbits.TCKPS = 0;   // Timer3 prescaler N=1
  PR3 = 3999;            // period = (PR3+1) * N * 12.5 ns = 50us. 20kHz
  TMR3 = 0;              // initial TMR3 count is 0
  OC1CONbits.OCTSEL = 1; // selects Timer3
  OC1CONbits.OCM =
      0b110;    // PWM mode without fault pin; other OC1CON bits are defaults
  OC1RS = 1000; // duty cycle = OC1RS/(PR2+1) = 25%
  OC1R = 1000;  // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;  // turn on Timer3
  OC1CONbits.ON = 1; // turn on OC1

  TRISDbits.TRISD0 = 0; // make D0 an output
  TRISDbits.TRISD11 = 0;
}

void makeWaveform() {
  int i = 0;
  int center = 0;
  int A = 200; // square wave, fill in center value and amplitude
  for (i = 0; i < NUMSAMPS; i++) {
    if (i < 25) {
      Waveform[i] = center + A;
    } else if (i >= 25 && i < 50) {
      Waveform[i] = center - A;
    } else if (i >= 50 && i < 75) {
      Waveform[i] = center + A;
    } else {
      Waveform[i] = center - A;
    }
  }
}

int curr_PI(float kp, float ki, int ref, int actual) {
  int error = ref - actual;
  curr_E_int = curr_E_int + error;

  if (error > 0) {
    dir = 1;
  } else {
    dir = 0;
  }
  int u = abs((kp * error) + (ki * curr_E_int));

  if (u > 100) {
    u = 100;
  }
  return (unsigned int)((u / 100.0) * 4000);
}

void set_direction(int d) { LATDbits.LATD11 = d; }