#include "NU32.h" // constants, funcs for startup and UART
#include "LCD.h"
#include "stdio.h"


#define delay 10000 // the bigger the delay the bigger time the user has to press the button to activate the interup
#define MSG_LEN 20
#define accuracy 4000000 

// global
float time = 0 ;

static volatile unsigned int flag = 0, startTime = 0; 
void __ISR(_EXTERNAL_0_VECTOR, IPL6SRS) Interup(void) { // step 1: the ISR
// void __ISR(_EXTERNAL_0_VECTOR, IPL2SOFT) Ext0ISR(void) { // step 1: the ISR

    _CP0_SET_COUNT(0);
    while(!PORTDbits.RD0){ // while button still pressed delay a little
        if(_CP0_GET_COUNT()> delay){ 
            break; 
        }
    }


    if(_CP0_GET_COUNT()>delay){ // if time is more thatn 1k then button was pressed from user (was not bounced)
        //open leds
        NU32_LED1 = 0; // LED1 and LED2 on
        NU32_LED2 = 0;

        flag = 1; 
        startTime = 1 ;

        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 10000000) { ; } // delay for 10 M core ticks, 0.25 s
        NU32_LED1 = 1; // LED1 and LED2 off
        NU32_LED2 = 1;
        IFS0bits.INT0IF = 0; // clear interrupt flag IFS0<3>

        
    }
}



// display time in screen
void showTime(int endTime){
    if(endTime==0){
        char msg[MSG_LEN];
        char msg_2[MSG_LEN];
        char msg_pc1[MSG_LEN];
        char msg_pc2[MSG_LEN];
        LCD_Move(0,0);
        sprintf(msg,"time --> %.3f",time);
        LCD_WriteString(msg);

        LCD_Move(1,0);
        sprintf(msg_2,"Press for STOP  ");
        LCD_WriteString(msg_2);


        // display time at computer
        sprintf(msg_pc1,"time --> %.3f \n\r",time);
        NU32_WriteUART3(msg_pc1);    
        sprintf(msg_pc2,"Press User BTN for Stop\n\r",time);
        NU32_WriteUART3(msg_pc2);  
    }
    else{
        char msg[MSG_LEN];
        char msg_2[MSG_LEN];
        char msg_pc1[MSG_LEN];
        char msg_pc2[MSG_LEN];
        LCD_Move(0,0);
        sprintf(msg,"Time Tot--> %.3f",time);
        LCD_WriteString(msg);

        LCD_Move(1,0);
        sprintf(msg_2,"Press for Reset");
        LCD_WriteString(msg_2);


        // display time at computer
        sprintf(msg_pc1,"Total Time --> %.3f \n\r",time);
        NU32_WriteUART3(msg_pc1);    
        sprintf(msg_pc2,"Press User BTN for Restart\n\r",time);
        NU32_WriteUART3(msg_pc2);   
    }


}

int main(void) {
    NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
    __builtin_disable_interrupts(); // step 2: disable interrupts
    INTCONbits.INT0EP = 0; // step 3: INT0 triggers on falling edge
    IPC0bits.INT0IP = 6; // step 4: interrupt priority 2
    IPC0bits.INT0IS = 1; // step 4: interrupt priority 1
    IFS0bits.INT0IF = 0; // step 5: clear the int flag
    IEC0bits.INT0IE = 1; // step 6: enable INT0 by setting IEC0<3>
    __builtin_enable_interrupts(); // step 7: enable interrupts


    LCD_Setup();
    LCD_Clear();                              // clear LCD screen

    _CP0_SET_COUNT(0);
    char start_msg1[25],start_msg2[25];;
    LCD_Move(0,0);
    sprintf(start_msg1,"Press USER BTN");
    LCD_WriteString(start_msg1);
    LCD_Move(1,0);
    sprintf(start_msg2,"for start!!");
    LCD_WriteString(start_msg2);

    TRISDbits.TRISD8 = 0; // set pin D8 to output (LED)
    LATDbits.LATD8 = 1; // set LED ON 

    while(1){
        if(startTime==1){
            break;
        }
    }

    int timer = 0;

    while(1) {
        
        if(timer==1){
            if(_CP0_GET_COUNT()>accuracy){ // count in 0.1 sec
                time = time + 0.1;
                _CP0_SET_COUNT(0);
                showTime(0); // display time in screen
            }
        }

        if(flag==1){ // condition for stop resume ( comes from interupt)
            //show end time
            _CP0_SET_COUNT(0);
            showTime(1);
            timer = !timer; // stop/resume timer
            flag = 0 ;
            time = 0 ;
            LATDbits.LATD8 = !LATDbits.LATD8;
        }

    }
        return 0;
}