#include <8051.h>

// 0 bit means that segment is on
__code unsigned char table[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82,
	0xf8, 0x80, 0x90
};

void delay(unsigned int i)
{
	unsigned char j;

	for (; i > 0; i--)
		for (j = 255; j > 0; j--);
}


void display(int v)
{   
    if (v == 10000) {
        v = 0;
    }

    if (v == -1) {
        v = 9999;
    }

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
	TMOD = 0x02;
	TH0 = 0x06;
	TL0 = 0x06;
	TR0 = 1;
	ET0 = 1;
	EA = 1;

	while (1) {
        display(4321);
	}
}
