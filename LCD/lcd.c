#include <8051.h>

#define SDA P0_0
#define SCL P0_1
#define INT P0_2
#define DBG P0_3
#define MSK 0x80
#define LEDMSK 0x04
#define ACK 0
#define NACK 1

// lcd display: address 3f | 7e 7f
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

void pcf_command
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

void cmdin(void) 
{
    for (int i = 7; i >= 0; i--) {
        SCL = 0;
        SCL = 1;
        delay(1);
        if (SDA) { // read port
            P1 = ~i; // 1;
        }
        else {
            P1 = 0xFF;
        }
        delay(10);
        SCL = 0;
    }
}


void start(void) {
    SDA = 1;
    delay(1);
    SCL = 1;
    delay(1);
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

// ack from slave
void wack(void) {
     // ACK from slave
        SCL = 1;
        delay(100);
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
}

// read ack from master
void rack(unsigned char valid) {
        SDA = valid;
        delay(1);
        SCL = 1;
        delay(10);
        // if (SDA) {  // NACK
        //     P1 = 0x00;
        //     delay(100);
        //     P1 = 0xFF;
        //     // stop();
        // } else { // SDA low is ACK
        //     P1 = 0xFF;
        //     delay(100);
        // }
        SCL = 0;
}

void lcd_write(unsigned char cmd) {
    cmdout(0x7e);
    delay(10);
    wack();
    cmdout(cmd);
    wack();
}

void lcd_read(void) {
    cmdout(0x7f);
    delay(10);
    wack();
    cmdin();
    rack(ACK);
}

void LCD_initialize(void) {

}


// https://www.youtube.com/watch?v=WAGLQOIHEio&ab_channel=ControllersTech
// upper nibble data bus lines d7 d6 d5 d4, for p7 p6 p5 p4 respectively     (bits 7 6 5 4)
// lower nibbles are backlight, enable, r/w, register select for p3 p2 p1 p0 respectively (bits 3 2 1 0)
// register select is high when writing to the lcd, low when sending a command
// r/w is only set high when reading from lcd
void main(void)  
{  
    delay_led(1000);
    

    while(1) {
        // LCD addresses: 7e write, 7f read
        start();
        lcd_write(0x0c);
        stop();
        delay_led(100);

        // start();
        // lcd_read();
        // stop();
        break;
        
        cmdout(0x38);  // dl = 8, lines = 2
        wack();

        cmdout(0x00);
        wack();
        cmdout(0x0c); // display on
        wack();

        cmdout(0x00);
        wack();
        cmdout(0x06); // cursor direction increments
        wack();

        cmdout(0x00);
        wack();
        cmdout(0x01); // clear displayand reset add to 0
        wack();

        cmdout(0x00);
        wack();
        cmdout(0x02); // reset cursor
        wack();

        stop();
        // delay_led(100000);

}

        // unsigned char cmd = 0x0F;
    //     unsigned char cmd = 0b00001100; // lower bytes three bits are r rw e 
    //     for (unsigned char i = 0; i < 8; i++) {
    //         if (cmd & 0x80) {
    //             SDA = 1;
    //         }
    //         else {
    //             SDA = 0;
    //         }
    //         delay(10);
    //         SCL = 1;
    //         cmd = cmd << 1;  // clock data into sda
    //         SCL = 0;
    //     }
    //     SCL = 0;

    //      // ACK from slave
    //     SCL = 1;
    //     delay(1);
    //     if (SDA) {  // NACK
    //         P1 = 0x00;
    //         delay(100);
    //         P1 = 0xFF;
    //         // stop();
    //     } else { // SDA low is ACK
    //         P1 = 0xFF;
    //         delay(100);
    //     }
    //     SCL = 0;
}

