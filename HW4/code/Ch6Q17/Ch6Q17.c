#include "NU32.h"          // constants, funcs for startup and UART
#include "LCD.h"
#define DELAYTIME 400000

int state = 0;

void debounce(){
  _CP0_SET_COUNT(0);
  while(_CP0_GET_COUNT() < DELAYTIME) {;}
}

void __ISR(_EXTERNAL_0_VECTOR, IPL2SOFT) Ext0ISR(void) { // step 1: the ISR
  _CP0_SET_COUNT(0);
  char stopString[100], elapsedTime[50];
  float et;
  sprintf(stopString, "Press the USER button again to stop the timer\n");
  NU32_WriteUART3(stopString);

  while (PORTDbits.RD9) { ; }
  
  et = _CP0_GET_COUNT()*12.5*1000000000;
  sprintf(elapsedTime, "%f seconds elapsed", et);
  NU32_WriteUART3(elapsedTime);
  IFS0bits.INT2IF = 0;            // clear interrupt flag IFS0<3>
}

int main(void) {
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  LCD_Setup();
  char startString[100];
  sprintf(startString,"Press the USER button to start the timer \r\n");
  NU32_WriteUART3(startString);
  __builtin_disable_interrupts(); // step 2: disable interrupts
  INTCONbits.INT2EP = 0;          // step 3: INT0 triggers on falling edge
  IPC2bits.INT2IP = 2;            // step 4: interrupt priority 2
  IPC2bits.INT2IS = 1;            // step 4: interrupt priority 1
  IFS0bits.INT2IF = 0;            // step 5: clear the int flag
  IEC0bits.INT2IE = 1;            // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts();  // step 7: enable interrupts
                                  // Connect RD7 (USER button) to INT0 (RD0)
  while(1) {
      ; // do nothing, loop forever
  }

  return 0;
}