/*  This program interfaces with the QX-mini51 STC89C52 MCU
    The STC89 uses an MCS-51(8051) series instruction set standard

    This program combines the BinaryCount, Cyclon, Dual Cyclon, and
    Display programs into one feature set. 
*/

#include <8051.h>
#include <stdio.h>

// 0 bit means that segment is on
// dictionary for digits 0-9 on the 7-segment display
__code unsigned char table[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82,
	0xf8, 0x80, 0x90
};

// K1 - K4 are keys S1-S4 respectively
#define	K4	P3_2
#define	K3	P3_3
#define	K2	P3_4
#define	K1	P3_5

#define OFF 0;
#define COUNT 1
#define CYCLON 2
#define DUAL 3

int count = 0; // counter for LEDs

// source: https://github.com/retiredfeline/QX-mini51-SDCC
// program a delay between operations
void delay(unsigned int i)
{
	unsigned char j;

	for (; i > 0; i--)
		for (j = 255; j > 0; j--);
}

// displays decimal 0-9999
void display_seg(int v, int mode)
{   

    // P2_0 - P2-3 are 1000s, 100s, 10s, 1s respectively
    // Gets each decimal digit and displays their 7-segment representation
	P0 = table[(v/1000)];
	P2_0 = 0;
	delay(5);
	P2_0 = 1;

	P0 = (table[v%1000/100] );
	P2_1 = 0;
	delay(5);
	P2_1 = 1;

	P0 = table[v%100/10];
	P2_2 = 0;
	delay(5);
	P2_2 = 1;

	P0 = table[v%10];
	P2_3 = 0;
	delay(5);
	P2_3 = 1;

    // if (mode == OFF) {
    //     P1 = 0;
    // }
    if (mode == COUNT) {

            P1 = ~count;
            count++;
            if (count == 255) {
                count = 0;
            }
            delay(5);
    }
    if (mode == CYCLON) {
        // 3 = 00111111 on LEDS; take inverse = 11000000 on LEDS
        // right to left on the cylon, then  left to right on the next loops
        
        for (int i = 0; i < 6; i++) {
            P1 = ~ (3<<i) ;
            delay(5);
        }
        for (int i = 0; i < 6; i++) {
            P1 = ~ (192>>i) ;
            delay(5);
        }
    }
    if (mode == DUAL) {
        if (count == 6) {
            count = 0;
        }
        P1 = ~(3<<count | 192>>count);
        delay(200);
    }

}

void main(void)
{
    int v = 0;
    int mode = OFF;

	while (1) {
        int t = 0; // var for button timer
        // 0 = button press
        // K3-K4 increments/decrements counter on 7-segment display
        if (K3 == 0) {
            delay(5);
            if (K3 == 0) {
                while(!K3) {  // Display current value if button is held
                    display_seg(v, mode);
                }
                v++;
                if (v > 9999) {
                    v = 0;
                }
            }
        }
        if (K4 == 0) {
            delay(5);
            if (K4 == 0) {
                while(!K4) {
                    display_seg(v, mode);
                }
                v--;
                if (v < 0) {
                    v = 9999;
                }
            }
        }
        display_seg(v, mode);

        // if (mode == COUNT) {

        //     P1 = ~count;
        //     count++;
        //     if (count == 255) {
        //         count = 0;
        //     }
        //     display_seg(v);
        //     delay(100);
        // }
        // if (mode == CYCLON) {
        //     // 3 = 00111111 on LEDS; take inverse = 11000000 on LEDS
        //     // right to left on the cylon, then  left to right on the next loops
        //     for (int i = 0; i < 6; i++) {
        //         P1 = ~ (3<<i) ;
        //         delay(300);
        //     }
        //     for (int i = 0; i < 6; i++) {
        //         P1 = ~ (192>>i) ;
        //         delay(300);
        //     }
        // }
        // if (mode == DUAL) {
        //     if (count == 6) {
        //         count = 0;
        //     }
        //     P1 = ~(3<<count | 192>>count);
        //     delay(200);
        // }
        // toggles between LED features
        if (K1 == 0) {
            delay(5);
            while(!K1) {
                display_seg(v, mode);
            }
            mode++;
            if (mode == 4) {
                mode = 0;
            }
            count = 0;
        }


	}
}