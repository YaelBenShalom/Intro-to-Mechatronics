#include "positioncontrol.h"
#include <xc.h>

void position_init(void) {
  // Configure Timer 4 for 200Hz interrupt:
  T4CONbits.TCKPS = 0b110; // Timer4 prescaler N=64 (1:64)
  PR4 = 6249;              // period = (PR2+1) * N * 12.5 ns = 5ms, 200Hz
  TMR4 = 0;                // initial TMR4 count is 0
  T4CONbits.ON = 1;        // turn on Timer4
  IPC4bits.T4IP = 5;       // priority
  IPC4bits.T4IS = 1;       // sub-priority
  IFS0bits.T4IF = 0;       // int flag
  IEC0bits.T4IE = 1;       // enable int
}

int position_PID(float pos_kp, float pos_ki, float pos_kd, int ref_pos,
                 int actual_pos) {
  int error = ref_pos - actual_pos;
  pos_E_int = pos_E_int + error;
  int pos_E_dot = error - pos_E_prev;
  pos_E_prev = error;

  int u = (pos_kp * error) + (pos_ki * pos_E_int) + (pos_kd * pos_E_dot);

  if (u > 300) {
    u = 300;
  } else if (u < -300) {
    u = -300;
  } else {
    ;
  }

  return u;
}