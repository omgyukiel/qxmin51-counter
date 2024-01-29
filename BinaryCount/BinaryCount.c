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
    // FF all LEDs OFF
    // 00 all LEDS ON
    // int d;
        for (int i = 0; i <255 ; i--) {
            // d = i;
            P1 = reverse(i); 
            delay(300);
            // P1 = ~i;
            // delay(800);
        }
        
        for (int i = 0; i < 5; i++) {
         P1 = 0x00; // Turn ON all LED's connected to Port1
         delay(300);
         P1 = 0xFF; // Turn OFF all LED's connected to Port1
         delay(300);
        }

}