#include <8051.h>

// 0 bit means that segment is on
__code unsigned char table[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82,
	0xf8, 0x80, 0x90
};

#define	K4	P3_2
#define	K3	P3_3
#define	K2	P3_4
#define	K1	P3_5

void delay(unsigned int i)
{
	unsigned char j;

	for (; i > 0; i--)
		for (j = 255; j > 0; j--);
}

// displays decimal 0-9999
void display(int v)
{   
    // if (v > 9999) {
    //     v = 0;
    // }

    // if (v < 0) {
    //     v = 9999;
    // }

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

}

void main(void)
{
    int v = 0;

	while (1) {
        // 0 = button press
        if (K3 == 0) {
            delay(5);
            if (K3 == 0) {
                while(!K3) {
                    display(v);
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
                    display(v);
                }
                v--;
                if (v < 0) {
                    v = 9999;
                }
            }
        }
        display(v);
	}
}
