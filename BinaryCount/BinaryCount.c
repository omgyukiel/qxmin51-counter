#include <8051.h>

/*  This program interfaces with the QX-mini51 STC89C52 MCU
    The STC89 uses an MCS-51(8051) series instruction set standard

    This program will flash the LED's 1-8 on the QX-mini to create a 
    binary counting pattern counting from 0-255.
*/

// source: https://github.com/retiredfeline/QX-mini51-SDCC
// program a delay between operations
void delay(unsigned int i)
{
	unsigned char j;

	for (; i > 0; i--)
		for (j = 255; j > 0; j--);
}

void main(void)
{
	P1 = 0x00;
	delay(600);
	P1 = 0xff;
	delay(600);
}