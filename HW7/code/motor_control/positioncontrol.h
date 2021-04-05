#ifndef POS_CONTROLLER
#define POS_CONTROLLER

volatile int pos_E_int;
volatile int pos_E_prev;

// Initialize the position module.
void position_init(void);

int position_PID(float pos_kp, float pos_ki, float pos_kd, int ref_pos, int actual_pos);

#endif