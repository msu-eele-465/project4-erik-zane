#ifndef CONTROL_LCD_H
#define CONTROL_LCD_H

#include <msp430.h>
#include <stdbool.h>
#include "shared.h"

void send_Latest_Input(char last_input);
void send_Pattern_Name(int Pattern);
void send_Blinking_toggle(int Toggle);
void send_Pattern_Speed(int new_speed);
void init_LCD_I2C(void); 

#endif // CONTROL_LCD_H