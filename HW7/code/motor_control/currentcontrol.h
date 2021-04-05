#ifndef currentcontrol__H__
#define currentcontrol__H__

#define NUMSAMPS 100

// volatile int duty_cycle;
volatile int curr_E_int;
volatile int dir; 

// Initialize the PWM module.
void PWM_init();

// Generate wave form
void makeWaveform();

int curr_PI(float curr_Kp, float curr_Ki, int ref, int actual);

// Set motor direction
void set_direction(int d);

#endif