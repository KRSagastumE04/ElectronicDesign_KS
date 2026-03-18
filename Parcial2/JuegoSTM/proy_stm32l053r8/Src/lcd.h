#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include "stm32l053xx.h"

void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char *s);
void lcd_print_line(uint8_t row, const char *s);

void lcd_queue_update(const char *line1, const char *line2);
void lcd_tick(void);

#endif
