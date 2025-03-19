#ifndef LCD_SHARED_H
#define LCD_SHARED_H

extern volatile unsigned int Last_char_int;
extern volatile char Last_char_char;
extern volatile unsigned int Pattern_name_int;
extern volatile unsigned int Blinking_toggle_state; // toggle blinking if 1, clear LCD if 0
extern volatile unsigned int Pattern_speed;

extern volatile unsigned int dataRead[2];
extern volatile unsigned int dataRead2[2];

#endif // LCD_SHARED_H