
#include <msp430.h>
#include "lcd_shared.h"
#include "lcd_commands.h"
#include <string.h>

// P1.2 is data pin (I2C), P1.3 is clock
volatile unsigned int Last_char_int;
volatile char Last_char_char;
volatile unsigned int Pattern_name_int;
volatile unsigned int Blinking_toggle_state; // toggle blinking if 1, clear LCD if 0
volatile unsigned int Pattern_speed;

volatile int index = 0;
volatile unsigned int dataRead[2] = {0, 0};
volatile unsigned int dataRead2[2] = {0, 0};
volatile int dataRdy = 0;
volatile int dataRdy2 = 0;
#define I2C_ADDRESS 0x47                //i2c address for LCD bar

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    P2OUT &= ~0b11000000;                   // 2.7, 2.6, 1.1, 1.0 will be MSB-LSB of 4 write bits
    P2DIR |= 0b11000000;
    P1OUT &= ~0b00110011;                    // P1.5 is Enable Pin
    P1DIR |= 0b00110011;                    // P1.4 is RS pin

    // I2C slave code
    UCB0CTLW0 = UCSWRST;                //puts eUSCI_B0 into a reset state
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;     //sets up synchronous i2c mode
    UCB0I2COA0 |= I2C_ADDRESS | UCOAEN;  //sets and enables address
    P1SEL0 |= BIT2 | BIT3; // Set P1.2 and P1.3 as I2C SDA and SCL
    P1SEL1 &= ~(BIT2 | BIT3); // this is missing from Zane's code
    UCB0CTLW0 &= ~UCSWRST;              
    UCB0IE |= UCRXIE0; 
    __enable_interrupt(); 
   // P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
    //P1DIR |= BIT0;                          // Set P1.0 to output direction

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings

    initLCD();
    clearLCD();

    while(1)
    {   
        if (dataRdy == 1 || dataRdy2 == 1) {
            unsigned int varint;
            unsigned int dataint;
            if (dataRdy == 1) {
                varint = dataRead[0];
                dataint = dataRead[1];
                dataRdy = 0;
            }
            else {
                varint = dataRead2[0];
                dataint = dataRead2[1];
                dataRdy2 = 0;
            }
            if(varint == 1) {
                Last_char_int = dataint;
            switch (Last_char_int) {
                case 1:  Last_char_char = '1'; break;
                case 2:  Last_char_char = '2'; break;
                case 3:  Last_char_char = '3'; break;
                case 4:  Last_char_char = 'A'; break;
                case 5:  Last_char_char = '4'; break;
                case 6:  Last_char_char = '5'; break;
                case 7:  Last_char_char = '6'; break;
                case 8:  Last_char_char = 'B'; break;
                case 9:  Last_char_char = '7'; break;
                case 10: Last_char_char = '8'; break;
                case 11: Last_char_char = '9'; break;
                case 12: Last_char_char = 'C'; break;
                case 13: Last_char_char = '*'; break;
                case 14: Last_char_char = '0'; break;
                case 15: Last_char_char = '#'; break;
                case 16: Last_char_char = 'D'; break;
                default: Last_char_char = 'I'; break; // Handles invalid values
            }

                goToDDRLCD(0x4F); // go to final character of second row
                writeChar(Last_char_char); // write our character
            }
            else if (varint == 2) {
                Pattern_name_int = dataint;
                goToDDRLCD(0x00); // go to first character of first row
                writeMessage("              ");
                goToDDRLCD(0x00);
                if (Pattern_name_int == 0) {
                    writeMessage("static");
                }
                else if (Pattern_name_int == 1) {
                    writeMessage("toggle");
                }
                else if (Pattern_name_int == 2) {
                    writeMessage("up counter");
                }
                else if (Pattern_name_int == 3) {
                    writeMessage("in and out");
                }
                else if (Pattern_name_int == 4) {
                    writeMessage("down counter");
                }
                else if (Pattern_name_int == 5) {
                    writeMessage("rotate 1 left");
                }
                else if (Pattern_name_int == 6) {
                    writeMessage("rotate 7 right");
                }
                else if (Pattern_name_int == 7) {
                    writeMessage("fill left");
                }
                else if (Pattern_name_int == 8) {
                    // clear LCD
                }
            }
            else if (varint == 3) {
                Blinking_toggle_state = dataint;
                // if 0, turn off LCD
                // if 1, toggle cursor blinking
                // if 2, toggle cursor
            }
            else if (varint == 4) {
                Pattern_speed = dataint;
                goToDDRLCD(0x40); // go to first character of first row
                writeMessage("Period=");
                unsigned char ones = (Pattern_speed/4) + '0';
                writeChar(ones);
                switch (Pattern_speed % 4) {
                    case 0: writeMessage(".00"); break;
                    case 1: writeMessage(".25"); break;
                    case 2: writeMessage(".50"); break;
                    case 3: writeMessage(".75"); break;
                }
            }
            else {
                // do something or nothing in case of invalid send
            }
        }
    }
}

#pragma vector = EUSCI_B0_VECTOR
__interrupt void I2C_ISR(void) {
    if (UCB0IFG & UCRXIFG0) {
        if (dataRdy == 1) {
            if (index == 1) {
                dataRead2[1] = UCB0RXBUF;
                index = 0;
                dataRdy2 = 1;
            }
            else {
                dataRead2[0] = UCB0RXBUF;
                index = 1;
            }
        }
        else {
            if (index == 1) {
                dataRead[1] = UCB0RXBUF;
                index = 0;
                dataRdy = 1;
            }
            else {
                dataRead[0] = UCB0RXBUF;
                index = 1;
            }
        }
    }
}