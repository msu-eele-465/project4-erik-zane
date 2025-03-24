#include <msp430.h>
#include "patterns.h"

#define I2C_ADDRESS 0x45                //i2c address for LED bar

volatile unsigned char dataReceived[2]; //buffer for received I2C data
volatile int dataRdy = 0;

void init_I2C_Target(void) {
    UCB0CTLW0 = UCSWRST;                //reset state
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;     //i2c
    UCB0I2COA0 = I2C_ADDRESS | UCOAEN;  //set and enable address

    P1SEL0 |= BIT2 | BIT3;              // select P1.2 (SDA) and P1.3 (SCL) for i2c
    P1SEL1 &= ~(BIT2 | BIT3);          

    UCB0CTLW0 &= ~UCSWRST;              //release eUSCI_B0 for operation
    UCB0IE |= UCRXIE0;                  //enable RX interrupt
}

void init_Timer(void) {
    TB0CCTL0 = CCIE;                    //enable interrupt for CCR0
    TB0CCR0 = 32768;                    //set CCR0 for ~1s
    TB0CTL = TBSSEL_1 | MC_1 | TBCLR;   //ACLK, Up mode, clear timer
}

void stop_Timer(void) {
    TB0CTL = MC_0;                      //stop timer
    TB0CCTL0 &= ~CCIE;                  //disable interrupt
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;           //stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;               //enable GPIOs

    init_I2C_Target();
    init_LED_Patterns();
    init_Timer();

    __enable_interrupt();               //global interrupt

    while (1) {
        if (dataRdy) {
            unsigned char var = dataReceived[0];
            unsigned char value = dataReceived[1];

            switch (var) {
                case 1:
                    set_Phase_Time(value * 6250);
                    break;
                case 2:
                    if (value == 1) {
                        init_Timer();
                    } else if (value == 0) {
                        stop_Timer();
                    }
                    break;
                case 3:
                    set_LED_Pattern(value);
                    break;
                default:                //unknown variable, do nothing
                    break;
            }

            dataRdy = 0;
        }
    }
}

#pragma vector = EUSCI_B0_VECTOR
__interrupt void I2C_ISR(void) {
    static int index = 0;
    if (UCB0IFG & UCRXIFG0) {
        dataReceived[index] = UCB0RXBUF;
        index = (index + 1) % 2;
        if (index == 0) dataRdy = 1;
    }
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void) {
    update_LED();
    TB0CCTL0 &= ~CCIFG;
}