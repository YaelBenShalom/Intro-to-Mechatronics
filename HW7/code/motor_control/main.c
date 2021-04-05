#include "NU32.h" // config bits, constants, funcs for startup and UART
# include "LCD.h"
#include "encoder.h"
#include "utilities.h"
#include "currentcontrol.h"
#include "positioncontrol.h"
#include "isense.h"

#include <stdio.h>
#include <xc.h>

#define BUF_SIZE 200


static volatile int counter = 0;
static volatile int I;
static volatile int StoringData = 0;


static volatile int duty_cycle = 0;
static volatile int deg_desired = 0;
static volatile int curr_Count = 0, curr_ref = 0;
// static volatile int pos_Count = 0, pos_ref = 0;
static volatile int curr_ref_test[99],curr_actual_test[99];
// static volatile int pos_ref_test[99],pos_actual_test[99];

static volatile int traj_No_Samples = 0, traj_i = 0, traj_actual[5000];

static volatile float kp = 0.06, ki = 0.0008;
static volatile float pos_kp = 75, pos_ki = 0, pos_kd = 5000;
static volatile float traj_ref[5000];


static volatile int Waveform[NUMSAMPS];   // waveform
static volatile int I_array[NUMSAMPS];    // measured values to plot
static volatile int REFarray[NUMSAMPS];   // reference values to plot


int main() {
  char buffer[BUF_SIZE];
  extern volatile modes MODE;

  NU32_Startup();   // cache on, min flash wait, interrupts on, LED/button init, UART init
  LCD_Setup();      // initialize LCD
  LCD_Clear();                    
  LCD_Move(0,0);
  NU32_LED1 = 1;    // turn off the LEDs
  NU32_LED2 = 1;    // turn off the LEDs

  __builtin_disable_interrupts();

  MODE = IDLE;
  encoder_init();
  ADC_init();
  PWM_init();
  position_init();
  __builtin_enable_interrupts();

  // while(1) {
  //   unsigned int adc = 0;
  //   adc = read_adc();
  //   sprintf(buffer,"%d\r\n", adc);
  //   NU32_WriteUART3(buffer);
  // }

  // while(1){
  //           int ticks = 0 ;
  //           ticks = encoder_angle();
  //           sprintf(buffer,"%d\r\n",ticks); // return the number + 1
  //           // display(buffer);
  //           NU32_WriteUART3(buffer);
  //   }

  while(1) {
    NU32_ReadUART3(buffer,BUF_SIZE);          // we expect the next character to be a
                                              // menu command
    NU32_LED2 = 1;                            // clear the error LED

    switch (buffer[0]) {
      case 'a': {                             // Read current sensor (ADC counts)
        unsigned int adc = 0;
        adc = read_adc();
        sprintf(buffer,"%d\r\n", adc);
        display(buffer);
        NU32_WriteUART3(buffer);
        break;
      }

      case 'b': {                             // Read current sensor (mA)
        int current = 0;
        current = read_current();
        sprintf(buffer,"%d\r\n", current);
        display(buffer);
        NU32_WriteUART3(buffer);
        break;
      }

      case 'c': {                             // Read encoder (counts)
        int count = 0;
        count = encoder_counts();
        sprintf(buffer, "%d\r\n", count);
        display(buffer);
        NU32_WriteUART3(buffer);
        break;
      }

      case 'd': {                             //  Read encoder (deg)
        int angle = 0;
        angle = encoder_angle();
        sprintf(buffer, "%d\r\n", angle);
        display(buffer);
        NU32_WriteUART3(buffer);
        break;
      }

      case 'e': {                             // Reset encoder
        sprintf(buffer, "%d\r\n", encoder_reset());
        display(buffer);
        NU32_WriteUART3(buffer);
        break;
      }

      case 'f': {                             // Set PWM (-100 to 100)
        MODE = PWM;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &duty_cycle);
        display(buffer);
        break;
      }

      case 'g': {                               //Set Current Gains
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f %f", &kp, &ki);
        display(buffer);
        break; 
      }

      case 'h': {                             //Get Current Gains
        sprintf(buffer,"%f %f\r\n", kp, ki);
        display(buffer);
        NU32_WriteUART3(buffer);
        break; 
      }

      case 'i': {                             // Set position gains
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f %f %f", &pos_kp, &pos_ki, &pos_kd);
        display(buffer);
        break;
      }

      case 'j': {                             // Get position gains
        sprintf(buffer,"%f %f %f\r\n", pos_kp, pos_ki, pos_kd);
        NU32_WriteUART3(buffer);
        break; 
      }

      case 'k': {                             // Test current control
        MODE = ITEST;
        while (MODE == ITEST) {;}

        sprintf(buffer,"%d\r\n", 99);
        display(buffer);
        NU32_WriteUART3(buffer);

        int i;
        for (i = 0; i < 99; i++) {
          sprintf(buffer,"%d %d\r\n", curr_ref_test[i], curr_actual_test[i]);
          display(buffer);
          NU32_WriteUART3(buffer);
        }
        break; 
      }

      // case 'y': {                             // Test current control
      //   MODE = ITEST;
      //   while (MODE == ITEST) {;}

      //   sprintf(buffer,"%d\r\n", 99);
      //   display(buffer);
      //   NU32_WriteUART3(buffer);

      //   int i;
      //   for (i = 0; i < 99; i++) {
      //     sprintf(buffer,"%d %d\r\n", deg_desired[i], actual_pos[i]);
      //     display(buffer);
      //     NU32_WriteUART3(buffer);
      //   }
      //   break; 
      // }

      case 'l': {                             //  Go to angle (deg)
        curr_E_int = 0;
        pos_E_int = 0;
        pos_E_prev = 0;
        NU32_ReadUART3(buffer, BUF_SIZE);
        display(buffer);
        sscanf(buffer, "%d", &deg_desired);
        MODE = HOLD;
        break;
      }

      case 'm': {                             //  Load step trajectory
        MODE = IDLE;
        NU32_ReadUART3(buffer, BUF_SIZE);
        display(buffer);
        sscanf(buffer, "%d", &traj_No_Samples);
        int i, temp = 0;
        for (i = 0; i < traj_No_Samples; i ++)
        {
          NU32_ReadUART3(buffer, BUF_SIZE);
          display(buffer);
          sscanf(buffer, "%d", &temp);
          traj_ref[i] = temp;
        }
        break;      }

      case 'n': {                             //  Load cubic trajectory
        MODE = IDLE;
        NU32_ReadUART3(buffer, BUF_SIZE);
        display(buffer);
        sscanf(buffer, "%d", &traj_No_Samples);
        int i = 0;        
        float temp = 0;

        for (i = 0; i < traj_No_Samples; i ++) {
          NU32_ReadUART3(buffer, BUF_SIZE);
          display(buffer);
          sscanf(buffer,"%f",&temp);
          traj_ref[i] = (int)temp;
        }
        break;
      }

      case 'o': {                              //Execute Trajectory
        MODE = TRACK;
        while (MODE == TRACK);
        sprintf(buffer, "%d\r\n", traj_No_Samples);
        NU32_WriteUART3(buffer);
        display(buffer);
        int i = 0;
        for (i = 0; i < traj_No_Samples; i ++) {
          sprintf(buffer, "%f %d\r\n", traj_ref[i], traj_actual[i]);
          NU32_WriteUART3(buffer);
          display(buffer);
        }
        break;
      }

      case 'p': {                           // Unpower the motor
        MODE = IDLE;
        get_mode(MODE);
        break;
      }

      case 'q': {                           // Quit client
        MODE = IDLE;
        break;
      }

      case 'r': {                           // Get mode
        get_mode(MODE);
        break;
      }

      case 'x': {                           // Test command
        int a = 0;
        int b = 0;

        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &a);
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &b);
        sprintf(buffer,"%d\r\n", (a + b));
        display(buffer);
        NU32_WriteUART3(buffer);
        break;
      }

      default: {
        NU32_LED2 = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}


void __ISR(_TIMER_2_VECTOR, IPL3SOFT) Controller(void) { // _TIMER_2_VECTOR = 8

  switch (MODE) {
    case IDLE: {
      duty_cycle = 0;
      OC1RS = 0;
      LATDbits.LATD0 = 0;
      break;
    }

    case PWM: {
      if (duty_cycle < 0) {
        if (duty_cycle < -100) {
          duty_cycle = -100;
        }
        OC1RS = -40 * duty_cycle;
        LATDbits.LATD0 = 1;
        set_direction(1);
      }

      if (duty_cycle >= 0) {
        if (duty_cycle > 100) {
          duty_cycle = 100;
        }
        LATDbits.LATD0 = 1;
        set_direction(0);
        OC1RS = 40 * duty_cycle;
      }
      break;
    }

    case ITEST: {
      if ((curr_Count > 25 && curr_Count < 50) || (curr_Count > 75)) {
        curr_ref_test[curr_Count] = (-1)*(200);
      }
      else {
        curr_ref_test[curr_Count] = 200;
      }
      curr_actual_test[curr_Count] = read_current();

      OC1RS = curr_PI(kp, ki, curr_ref_test[curr_Count], curr_actual_test[curr_Count]);
      set_direction(dir);

      if (curr_Count == 99) {
        curr_Count = 0;
        MODE = IDLE;
        curr_E_int = 0;
        break;
      }

      curr_Count++;
      break;
    }

    // case POSTEST: {
    //   if ((pos_Count > 25 && pos_Count < 50)||(pos_Count > 75)) {
    //     pos_ref_test[pos_Count] = (-1)*(200);
    //   }
    //   else {
    //     pos_ref_test[pos_Count] = 200;
    //   }
    //   curr_actual_test[pos_Count] = read_current();

    //   OC1RS = position_PID(pos_kp, pos_ki, pos_kd, pos_ref_test[pos_Count], pos_actual_test[pos_Count]);
    //   set_direction(dir);

    //   if (pos_Count == 99) {
    //     pos_Count = 0;
    //     pos_E_int = 0;
    //     MODE = IDLE;
    //     break;
    //   }

    //   pos_Count++;
    //   break;
    // }

    case HOLD: {
      if (curr_ref < 0) {
        dir = 0;
      }
      else {
        dir = 1;
      }
      int curr_actual = read_current();
      OC1RS = curr_PI(kp, ki, curr_ref, curr_actual);
      set_direction(dir);
      break;
    }

    case TRACK: { 
      if (curr_ref < 0) {
        dir = 0;
      }
      else {
        dir = 1;
      }
      int curr_actual = read_current();
      OC1RS = curr_PI(kp, ki, curr_ref, curr_actual);
      set_direction(dir);
      break;
    }
  }

  IFS0bits.T2IF = 0;                      // clear interrupt flag IFS0
}

void __ISR(_TIMER_4_VECTOR, IPL5SOFT) positioncontrol(void) { //Timer 4

  if (MODE == HOLD) {
    int actual_pos = (int)encoder_angle();
    curr_ref = position_PID(pos_kp, pos_ki, pos_kd, deg_desired, actual_pos);
  }

  if (MODE == TRACK) {
    int actual_pos = (int)encoder_angle();
    traj_actual[traj_i] = actual_pos;
    curr_ref = position_PID(pos_kp, pos_ki, pos_kd, traj_ref[traj_i], traj_actual[traj_i]);

    if(traj_i == traj_No_Samples) {
      curr_E_int = 0;
      pos_E_int = 0;
      pos_E_prev = 0;
      traj_i = 0;
      deg_desired = actual_pos;
      MODE = HOLD;
    }
    else {
      traj_i++;
    }
  }

  IFS0bits.T4IF = 0;       //int flag
}