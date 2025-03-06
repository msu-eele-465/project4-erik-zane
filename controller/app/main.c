#include <msp430.h>
#include <stdbool.h>
#include <string.h>
#include "controlPatternsLED.h"
#include "keypad.h"
#include "RGB.h"
#include "shared.h"

#define RED_LED   BIT1 
#define GREEN_LED BIT2 
#define BLUE_LED  BIT3 

volatile unsigned int red_counter = 0;
volatile unsigned int green_counter = 0;
volatile unsigned int blue_counter = 0;

volatile unsigned int dataSend[2] = {0, 0};
int Data_Cnt = 0;

volatile unsigned int limit_reached = 0;
volatile unsigned int pwms = 0;

volatile system_states state = LOCKED;

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    P3DIR |= 0b00001111;   // set keypad columns to outputs pulled high
    P3OUT |= 0b00001111;

    //heartbeat LED
    P1DIR |= BIT0;                              // Sets P1.0 as an output
    P1OUT &= ~BIT0;                             // Initializes LED to OFF

    P3DIR &= ~0b11110000; // set all keypad rows to inputs pulled low
    P3REN |= 0b11111111; // permanently set all of port 3 to have resistors
    P3OUT &= ~0b11110000; // pull down resistors

    P1DIR |= RED_LED | GREEN_LED | BLUE_LED;
    P1OUT |= RED_LED | GREEN_LED | BLUE_LED;  // Start with all ON

    //heartbeat interrupt
    TB1CCTL0 = CCIE;                            //CCIE enables Timer B0 interrupt
    TB1CCR0 = 32768;                            //sets Timer B0 to 1 second (32.768 kHz)
    TB1CTL = TBSSEL_1 | ID_0 | MC__UP | TBCLR;    //ACLK, No divider, Up mode, Clear timer


    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configure port settings
    init_LED_I2C() // what it says, but this also likely works for LCD controller

    PM5CTL0 &= ~LOCKLPM5;

    set_timer(); 

    __enable_interrupt(); 
    //__enable_interrupt(); 
    int unlock = 0;

    while(true)
    {
        unlock = 0;
        state = LOCKED;
        update_color(state);
        while (unlock == 0) {
            unlock = waitForUnlock(); // stays here until complete passkey has been entered 
        }
        // turn status LED Blue here
        state = UNLOCKED;
        update_color(state);
        char lastInput = 'X';
        while (lastInput != '0' && lastInput != '1' && lastInput != '2' && lastInput != '3' && lastInput != '4' && lastInput != '5' && lastInput != '6' && lastInput != '7' && lastInput != 'D') {
            lastInput = readInput(); // stays here until user chooses a pattern, or chooses to lock the system
        }
        int rows;
        int phaseTime = 25000; // 
        send_LED_Phase_Delay(phaseTime);
        send_LED_Timer_Set(); // enable LED-pattern-trigger timer interrupt here
        while (lastInput != 'D') {
            if (lastInput == '1') {
                send_LED_Pattern(1);
                rows = P3IN; // constantly listen for an input
                rows &= 0b11110000; // clear any values on lower 4 bits
                if (rows != 0b00000000) {
                    lastInput = readInput();
                }
            }     
            else if (lastInput == 'A' && phaseTime > 7000) {
                phaseTime = phaseTime - 6250;
                send_LED_Phase_Delay(phaseTime);
                rows = P3IN; // constantly listen for an input
                rows &= 0b11110000; // clear any values on lower 4 bits
                if (rows != 0b00000000) {
                    lastInput = readInput();
                }
            }
            else if (lastInput == 'B') {
                phaseTime = phaseTime + 6250;
                send_LED_Phase_Delay(phaseTime);
                rows = P3IN; // constantly listen for an input
                rows &= 0b11110000; // clear any values on lower 4 bits
                if (rows != 0b00000000) {
                    lastInput = readInput();
                }
            }
                
            else if (chosenPattern == '2') {
                send_LED_Pattern(2);
                rows = P3IN; // constantly listen for an input
                rows &= 0b11110000; // clear any values on lower 4 bits
                if (rows != 0b00000000) {
                    lastInput = readInput();
                }
            }
            else if (chosenPattern == '3') {
                send_LED_Pattern(3);
                rows = P3IN; // constantly listen for an input
                rows &= 0b11110000; // clear any values on lower 4 bits
                if (rows != 0b00000000) {
                    lastInput = readInput();
                }
            }

            else if (chosenPattern == '0') {
                send_LED_Pattern(0);
                rows = P3IN; // constantly listen for an input
                rows &= 0b11110000; // clear any values on lower 4 bits
                if (rows != 0b00000000) {
                    lastInput = readInput();
                }
            }

            else if (chosenPattern == '4') {
                send_LED_Pattern(4);
                rows = P3IN; // constantly listen for an input
                rows &= 0b11110000; // clear any values on lower 4 bits
                if (rows != 0b00000000) {
                    lastInput = readInput();
                }
            }

            else if (chosenPattern == '5') {
                send_LED_Pattern(5);
                rows = P3IN; // constantly listen for an input
                rows &= 0b11110000; // clear any values on lower 4 bits
                if (rows != 0b00000000) {
                    lastInput = readInput();
                }
            }

            else if (chosenPattern == '6') {
                send_LED_Pattern(6);
                rows = P3IN; // constantly listen for an input
                rows &= 0b11110000; // clear any values on lower 4 bits
                if (rows != 0b00000000) {
                    lastInput = readInput();
                }
            }

            else if (chosenPattern == '7') {
                send_LED_Pattern(7);
                rows = P3IN; // constantly listen for an input
                rows &= 0b11110000; // clear any values on lower 4 bits
                if (rows != 0b00000000) {
                    lastInput = readInput();
                }
            }

            else {
                rows = P3IN; // constantly listen for an input
                rows &= 0b11110000; // clear any values on lower 4 bits
                if (rows != 0b00000000) {
                    lastInput = readInput();
                }
            }
        }
        send_LED_Pattern(0);
        send_LED_Timer_pause(); // disable LCD-pattern-trigger timer interrupt here (system returns to locked state)
    }
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TimerB0_ISR(void) {

    pwms = (pwms + 1) % 256;

    // Red LED
    if (pwms == red_counter){
        P1OUT &= ~RED_LED;
    }
    if (pwms == 0) {
        P1OUT |= RED_LED;
    }

    // Green LED
    if (pwms == green_counter) {
        P1OUT &= ~GREEN_LED;
    }
    if (pwms == 0) {
        P1OUT |= GREEN_LED;
    }

    // Blue LED
    if (pwms == blue_counter) {
        P1OUT &= ~BLUE_LED;
    }
    if (pwms == 0) {
        P1OUT |= BLUE_LED;
    }
    TB0CCTL0 &= ~CCIFG;

}

#pragma vector = TIMER1_B0_VECTOR               //time B0 ISR
__interrupt void TIMERB1_ISR(void) {
    P1OUT ^= BIT0;                              //toggles P1.0 LED
    TB1CCTL0 &= ~CCIFG;
}

#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void) {
    if (Data_Cnt == 0) {
        UCB0TXBUF = dataSend[0];
        Data_Cnt = 1;
    }
    else {
        UCB0TXBUF = dataSend[1];
        Data_Cnt = 0;
    }
    // likely need to clear interrupt flag following this
}