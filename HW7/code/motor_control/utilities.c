#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "utilities.h"

#include <stdio.h>
#include <xc.h>


int display(char* msg) {
  LCD_Clear();                    
  LCD_Move(0,0);
  // sprintf(msg);          // print to LCD
  LCD_WriteString(msg);
}

modes get_mode(modes m){
  char buffer[30];

  switch (m) {

    case IDLE: {                    // In IDLE mode, the PIC32 puts the H-bridge in 
                                    // brake mode, with zero voltage across the motor
      sprintf(buffer,"Mode set: IDLE\r\n");
      NU32_WriteUART3(buffer);            
      break;
    }

    case PWM: {                     // In PWM mode, the PIC32 implements a fxed 
                                    // PWM duty cycle, as requested by the user
      sprintf(buffer,"Mode set: PWM\r\n");
      NU32_WriteUART3(buffer);            
      break;
    }

    case ITEST: {                   // In ITEST mode, the PIC32 tests the current control
                                    // loop, without caring about the motion of the motor.
      sprintf(buffer,"Mode set: ITEST\r\n");
      NU32_WriteUART3(buffer);
      break;
    }

    // case POSTEST: {                   // In POSTEST mode, the PIC32 tests the current control
    //                                 // loop, without caring about the motion of the motor.
    //   sprintf(buffer,"Mode set: POSTEST\r\n");
    //   NU32_WriteUART3(buffer);
    //   break;
    // }

    case HOLD: {                    // In HOLD mode, the PIC32 attempts to hold the motor
                                    // at the last position commanded by the user
      sprintf(buffer,"Mode set: HOLD\r\n");
      NU32_WriteUART3(buffer);
      break;
    }

    case TRACK: {                   // In TRACK mode, the PIC32 attempts to track a
                                    // reference motor trajectory specifed by the user
      sprintf(buffer,"Mode set: TRACK\r\n");
      NU32_WriteUART3(buffer);          
      break;
    }
  }
  return m;
}
