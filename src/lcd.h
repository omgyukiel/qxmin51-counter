#ifndef LCD_H 
#define LCD_H
#include <stdint.h>
void pcf_write_command(uint8_t *commands, int bytes);

void wack(void);

void lcd_cmd(char cmd);

void lcd_write(char cmd);

void lcd_send_string (char *str);
#endif