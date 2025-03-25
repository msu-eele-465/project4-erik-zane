#include <msp430.h>
#include "patterns.h"

#define LED_PORT P1OUT
#define LED_DIR P1DIR

int phaseTime = 25000;
int currPattern = 0;

void init_LED_Patterns(void) {                  //initializes LED bar pins as outputs and turns them all off
    LED_DIR |= 0xFF;                            
    LED_PORT &= ~0xFF;                          
}

void set_LED_Pattern(int pattern) {
    currPattern = pattern;
    switch (pattern) {
        case 0:
            LED_PORT = 0b10101010;              //this is the static pattern
            break;
        case 1:
            LED_PORT ^= 0xFF;                   //this is the toggle pattern
            break;
        case 2:
            LED_PORT = (LED_PORT + 1) & 0xFF;   //this is the up counter
            break;
        case 3:
                                                //this is the pattern logic for in and out Implement in/out pattern logic
            break;
        default:
            LED_PORT = 0;                       //the default will be set to off
            break;
    }
}

void set_Phase_Time(int time) {
    phaseTime = time;
}

void update_LED(void) {                         //updates the LED
    if (currPattern == 1) {
        LED_PORT ^= 0xFF;
    } else if (currPattern == 2) {
        LED_PORT = (LED_PORT + 1) & 0xFF;
    }
}