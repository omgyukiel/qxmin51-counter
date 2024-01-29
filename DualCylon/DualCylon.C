#include <8051.h>

/*  This program interfaces with the QX-mini51 STC89C52 MCU
    The STC89 uses an MCS-51(8051) series instruction set standard

    This program will flash the LED's 1-8 on the QX-mini to create a 
    dual cylon pattern.
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
    while(1) {
        for (int i = 0; i < 6; i++) {
            P1 = ~(3<<i | 192>>i);
            delay(300);
        }
    }
}