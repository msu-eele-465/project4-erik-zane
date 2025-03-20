#ifndef LCD_SHARED_H
#define LCD_SHARED_H

#include <stdint.h>

extern volatile uint8_t Last_char_int;
extern volatile char Last_char_char;

extern volatile uint8_t dataRead[2];
extern volatile uint8_t dataRead2[2];

#endif // LCD_SHARED_H