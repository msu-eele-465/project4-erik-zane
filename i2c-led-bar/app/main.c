#include <msp430.h>
#include "patterns.h"

#define I2C_ADDRESS 0x45                //i2c address for LED bar

volatile unsigned char dataReceived[2]; //this provides a buffer for received I2C data
volatile int dataRdy = 0;

void init_I2C_Target(void) {
    UCB0CTLW0 = UCSWRST;                //puts eUSCI_B0 into a reset state
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;     //sets up synchronous i2c mode
    UCB0I2COA0 = I2C_ADDRESS | UCOAEN;  //sets and enables address
    UCB0CTLW0 &= ~UCSWRST;              
    UCB0IE |= UCRXIE0;                  //enables the receive interrupt
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;           //stops the watchdog timer
    PM5CTL0 &= ~LOCKLPM5;               //enables the GPIO

    init_I2C_Target();
    init_LED_Patterns();

    __enable_interrupt();
    
    while (1) {
        if (dataRdy) {
            set_LED_Pattern(dataReceived[0]);
            set_Phase_Time(dataReceived[1] * 6250);
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
