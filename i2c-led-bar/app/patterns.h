#ifndef PATTERNS_H
#define PATTERNS_H

#include <msp430.h>
#include <stdbool.h>

//variables are pattern and time

void init_LED_Patterns(void);
void set_LED_Pattern(int pattern);
void set_Phase_Time(int time);
void update_LED(void);

#endif // PATTERNS_H
