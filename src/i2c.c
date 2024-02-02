#include <8051.h>
#include "periphs.h"
#include "constants.h"

// source: https://github.com/retiredfeline/QX-mini51-SDCC
// delay for 18  µs
void delay(unsigned int t)
{
	while (--t); // roughly i * 8 + 10 µs
}
// program a delay between operations
void delay_led(unsigned int i)
{
	unsigned char j;

	for (; i > 0; i--)
		for (j = 255; j > 0; j--);
}

// start condition for i2c op
void start(void) {
    SDA = 1;
    delay(1);
    SCL = 1;
    SDA = 0;
    delay(1);
    SCL = 0;
}

// stop condition for i2c op
void stop(void) {
    SDA = 0;
    delay(1);
    SCL = 1;
    SDA = 1;
}

// sends byte instruction to i2c
void cmdout(unsigned char cmd) 
{
    for (unsigned char i = 0; i < 8; i++) {
        if (cmd & MSK) {
            SDA = 1;
        }
        else {
            SDA = 0;
        }
        delay(10);
        SCL = 1;
        cmd = cmd << 1;  // clock data into sda
        SCL = 0;
    }
}

