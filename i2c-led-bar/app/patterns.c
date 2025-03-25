#include <msp430.h>
#include "patterns.h"

#define LED_PORT P2OUT
#define LED_DIR P2DIR

static int phaseTime = 25000;
static unsigned char currentPattern = 0;

void init_LED_Patterns(void) {
    LED_DIR |= BIT6 | BIT7;     // Only use P2.6 and P2.7
    LED_PORT &= ~(BIT6 | BIT7); // Clear output
}

void set_LED_Pattern(int pattern) {
    currentPattern = pattern;
    switch (pattern) {
        case 0:
            LED_PORT = (LED_PORT & ~(BIT6 | BIT7)) | BIT6;  // Example: light only P2.6
            break;
        case 1:
        case 2:
        case 3:
            // Animated patterns handled in timer ISR
            break;
        default:
            LED_PORT &= ~(BIT6 | BIT7); // turn off both LEDs
            break;
    }
}

void set_Phase_Time(int time) {
    phaseTime = time;
}

void update_LED(void) {
    // Not used; all animation logic is now in TIMER ISR in main.c
}

unsigned char get_Current_Pattern(void) {
    return currentPattern;
}
