#include <8051.h>

#define SDA P0_0
#define SCL P0_1
#define INT P0_2
#define DBG P0_3
#define MSK 0x80
#define LEDMSK 0x04

// delay for 18  µs
void delay(unsigned int t)
{
	while (--t); // roughly i * 8 + 10 µs
}	

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
    SCL = 0;
}

void lcd_set(unsigned char cmd) 
{
    for (unsigned char i = 0; i < 3; i++) {
        if (cmd & LEDMSK) {
            SDA = 1;
        }
        else {
            SDA = 0;
        }
        delay(10);
    }

        SCL = 1;
        cmd = cmd << 1;  // clock data into sda
        SCL = 0;
    
    // SDA = 0;
    SCL = 0;
}

// void cmdin(void) 
// {
//     for (int i = 7; i >= 0; i--) {
//         SCL = 0;
//         SCL = 1;
//         delay(1);
//         if (SDA) { // read port
//             P1 = ~i; // 1;
//         }
//         else {
//             P1 = 0xFF;
//         }
//         delay(10);
//         SCL = 0;
//     }
// }


void start(void) {
    SDA = 1;
    delay(1);
    SCL = 1;
    SDA = 0;
    delay(1);
    SCL = 0;
}

void stop(void) {
    SDA = 0;
    delay(1);
    SCL = 1;
    SDA = 1;
}

void delay_led(unsigned int i)
{
	unsigned char j;

	for (; i > 0; i--)
		for (j = 255; j > 0; j--);
}

void main(void) 
{  


    while(1) {


        // start();
        // cmdout(0x71); // read i2c on LCD 
        // // ACK from slave
        // SCL = 0;
        // SCL = 1;
        // delay(1);
        // if (SDA) {  // NACK
        //     // P1 = 0x00;
        //     // delay(10);
        //     // stop();
        //     P1 = 0xfe;
        //     delay_led(200);
        //     P1 = 0xFF;
        //     delay_led(200);

        // } else { // SDA low is ACK
        //     // P1 = 0x00;
        //     // delay(10);
        //     P1 =  0x00;
        //     delay_led(200);
        //     P1 = 0xFF;
        //     delay_led(200);
        // }
        // SCL = 0;

        // for (int i = 7; i >= 0; i--) {
        //     // SCL = 0;
        //     SCL = 1;
        //     delay(1);
        //     if (SDA) { // read port
        //         // P1 = ~i; // 1;
        //     } else {
        //         // delay(100);
        //         // P1 = 0xFF;
        //     }   
        //     SCL = 0;
        //     // delay(1);
        // }
        

        // // ACK from master
        // SDA = 0;
        // delay(1);
        // SCL = 1;
        // delay(1);
        // SCL = 0;

        // stop(); // 1st byte read
        // delay_led(500);



        start();
        cmdout(0x7e); // write 

        // ACK from slave
        SCL = 1;
        delay(1);
        if (SDA) {  // NACK
            P1 = 0x00;
            delay(100);
            P1 = 0xFF;
            // stop();
        } else { // SDA low is ACK
            P1 = 0xFF;
            delay(100);
        }
        SCL = 0;
        // lcd_set(0x00);

        // unsigned char cmd = 0x0F;
        unsigned char cmd = 0b10000000; // lower bytes three bits are r rw e 
        for (unsigned char i = 0; i < 8; i++) {
            if (cmd & 0x80) {
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
        SCL = 0;

         // ACK from slave
        SCL = 1;
        delay(1);
        if (SDA) {  // NACK
            P1 = 0x00;
            delay(100);
            P1 = 0xFF;
            // stop();
        } else { // SDA low is ACK
            P1 = 0xFF;
            delay(100);
        }
        SCL = 0;

        cmd = 0b00000111; 
        for (unsigned char i = 0; i < 8; i++) {
            if (cmd & 0x80) {
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
        SCL = 0;
    stop();
        // cmdout(0xFF); // write to ports

        // // ACK from slave
        // SCL = 1;
        // delay(1);
        // if (SDA) {  // NACK
        //     P1 = 0X00;
        //     delay(100);
        //     P1 = 0xFF;
        //     delay(100);
        //     // stop();
        // } else { // SDA low is ACK
        //     P1 = 0xFF;
        //     delay(100);
        // }
        // SCL = 0;

        // cmdout(0x00); // write to ports

        // // ACK from slave
        // SCL = 1;
        // delay(1);
        // if (SDA) {  // NACK
        //     P1 = 0X00;
        //     delay(100);
        //     P1 = 0xFF;
        //     delay(100);
        //     // stop();
        // } else { // SDA low is ACK
        //     P1 = 0xFF;
        //     delay(100);
        // }
        // SCL = 0;

     
        // delay_led(500);

        // cmdout(0xFF);
        // // ACK from slave
        // SCL = 1;
        // delay(1);
        // if (SDA) {  // NACK
        //     // P1 = 0x00;
        //     // delay(10);
        //     // stop();
        // } else { // SDA low is ACK
        //     // P1 = 0xFF;
        //     // delay(100);
        //     // stop();
        // }
        // SCL = 0;


        // stop();
    }



}

