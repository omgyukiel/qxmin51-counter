#include <8051.h>
#include <stdbool.h>

/*  This program interfaces with the QX-mini51 STC89C52 MCU
    The STC89 uses an MCS-51(8051) series instruction set standard

    This program will flash the LED's 1-8 on the QX-mini to create a 
    cylon pattern.
*/

// source: https://github.com/retiredfeline/QX-mini51-SDCC
// program a delay between operations
void delay(unsigned int i)
{
	unsigned char j;

	for (; i > 0; i--)
		for (j = 255; j > 0; j--);
}

// reverses bits
// source: https://stackoverflow.com/a/2602885
unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void main(void)
{
    while(1) {
        // 3 = 00111111 on LEDS; take inverse = 11000000 on LEDS, then reverse for 00000011
        // left to right on the cylon, then right to left on the next loops
        for (int i = 0; i < 6; i++) {
            P1 = reverse(~ (3<<i) );
            delay(300);
        }
        for (int i = 0; i < 6; i++) {
            P1 = reverse(~ (192>>i) );
            delay(300);
        }
    }
}