#include <stdint.h>
#include <8051.h>
#include "i2c.h"
#include "periphs.h"
#include "constants.h"


// ack from slave
void wack(void) {
     // ACK from slave
        SCL = 1;
        delay(1);
        if (SDA) {  // NACK
        // do something
        } else { // SDA low is ACK
        // do something
        }
        SCL = 0;
}
// write bytes amount of commands to i2c
void pcf_write_command(uint8_t *commands, int bytes) {
    start(); // start condition for i2c device;
    cmdout(0x7e); // set i2c at address 3f in write mode
    delay(1);
    wack(); // receive ack/nack from slave
    for (int i = 0; i < bytes; i++) { // SEND all commands in array
        cmdout(commands[i]);
        delay(1);
        wack(); // receive ack/nack from slave
    }
    stop(); // stop condition for i2c device;
}
/* https://www.youtube.com/watch?v=WAGLQOIHEio&ab_channel=ControllersTech
   upper nibble data bus lines d7 d6 d5 d4, for p7 p6 p5 p4 respectively     (bits 7 6 5 4)
   lower nibbles are backlight, enable, r/w, register select for p3 p2 p1 p0 respectively (bits 3 2 1 0)
   register select is high when writing to the lcd, low when sending a command
   commands must be clocked or "strobed" by setting the enable line off and off,so each command is sent twice to the lcd
   r/w is only set high when reading from lcd 
   note 1 byte = P7 P6 P5 P4 P3 P2 P1 P0  == D7 D6 D5 D4 LED E R/W RS
   */
// sends command to lcd through i2c 
void lcd_cmd(char cmd) 
{
    char upper, lower; // upper, lower nibbles
    uint8_t data_t[4];
    int bytes = 4;
    upper = (cmd&0xf0); // upper nibble
    lower = ( (cmd<<4) & 0xf0); // lower nibble
    // strobe commands by togglign enable line, rs = 0 to signal a command
    data_t[0] = upper | 0x0c; // en = 1 , rs = 0
    data_t[1] = upper | 0x08; // en = 0 , rs = 0
    data_t[2] = lower | 0x0c; 
    data_t[3] = lower | 0x08;
    pcf_write_command(data_t, bytes); 
}
void lcd_write(char cmd) 
{
    char upper, lower; // upper, lower nibbles
    uint8_t data_t[4];
    int bytes = 4;;
    upper = (cmd&0xf0); // upper nibble
    lower = ( (cmd<<4) & 0xf0); // lower nibble
    // strobe commands by togglign enable line, rs = 0 to signal a command
    data_t[0] = upper | 0x0d; // en = 1 , rs = 1
    data_t[1] = upper | 0x09; // en = 0 , rs = 1
    data_t[2] = lower | 0x0d; 
    data_t[3] = lower | 0x09; 
    pcf_write_command(data_t, bytes); 
}
void lcd_send_string (char *str)
{
	while (*str) lcd_write (*str++);
}
