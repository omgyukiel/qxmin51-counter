/*  This program interfaces with the QX-mini51 STC89C52 MCU
    The STC89 uses an MCS-51(8051) series instruction set standard

    This program combines the BinaryCount, Cyclon, Dual Cyclon, and
    Display programs into one feature set. 

    This program additonally multiplex 4 push buttons to light up LEDs on press.
    The buttons and leds are hooked up to an active-low i2c (PCF8575TS).
    Whent he button is pressed, a low signal is set on the i2c's port, and the 
    MCU (the QX-mini) will set the respective led's port level to low. The buttons
    activate the respective ports on the next byte within the i2c. EX. P0 == P10
*/
#include <8051.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
// K1 - K4 are keys S1-S4 respectively
#define	K4	P3_2
#define	K3	P3_3
#define	K2	P3_4
#define	K1	P3_5

#define SEG 0
#define COUNT 1
#define CYCLON 2
#define DUAL 3
#define SDA P0_0
#define SCL P0_1
#define INT P0_2
#define DBG P0_3
#define MSK 0x80
#define ACK 0
#define NACK 1
// 0 bit means that segment is on
// dictionary for digits 0-9 on the 7-segment display
__code unsigned char table[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82,
	0xf8, 0x80, 0x90
};




// program a delay between operations
void delay_led(unsigned int i)
{
	unsigned char j;

	for (; i > 0; i--)
		for (j = 255; j > 0; j--);
}

// source: https://github.com/retiredfeline/QX-mini51-SDCC
// delay for 18  µs
void delay(unsigned int t)
{
	while (--t); // roughly i * 8 + 10 µs
}	

// displays decimal 0-9999
void display_seg(int v)
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
// ack from slave
void wack(void) {
     // ACK from slave
        SCL = 1;
        delay(1);
        if (SDA) {  // NACK
        // do something
        } else { // SDA low is ACK
        // do something
        }
        SCL = 0;
}
// write bytes amount of commands to i2c
void pcf_write_command(uint8_t *commands, int bytes) {
    start(); // start condition for i2c device;
    cmdout(0x7e); // set i2c at address 3f in write mode
    delay(1);
    wack(); // receive ack/nack from slave
    for (int i = 0; i < bytes; i++) { // SEND all commands in array
        cmdout(commands[i]);
        delay(1);
        wack(); // receive ack/nack from slave
    }
    stop(); // stop condition for i2c device;
}
/* https://www.youtube.com/watch?v=WAGLQOIHEio&ab_channel=ControllersTech
   upper nibble data bus lines d7 d6 d5 d4, for p7 p6 p5 p4 respectively     (bits 7 6 5 4)
   lower nibbles are backlight, enable, r/w, register select for p3 p2 p1 p0 respectively (bits 3 2 1 0)
   register select is high when writing to the lcd, low when sending a command
   commands must be clocked or "strobed" by setting the enable line off and off,so each command is sent twice to the lcd
   r/w is only set high when reading from lcd 
   note 1 byte = P7 P6 P5 P4 P3 P2 P1 P0  == D7 D6 D5 D4 LED E R/W RS
   */
// sends command to lcd through i2c 
void lcd_cmd(char cmd) 
{
    char upper, lower; // upper, lower nibbles
    uint8_t data_t[4];
    int bytes = 4;
    upper = (cmd&0xf0); // upper nibble
    lower = ( (cmd<<4) & 0xf0); // lower nibble
    // strobe commands by togglign enable line, rs = 0 to signal a command
    data_t[0] = upper | 0x0c; // en = 1 , rs = 0
    data_t[1] = upper | 0x08; // en = 0 , rs = 0
    data_t[2] = lower | 0x0c; 
    data_t[3] = lower | 0x08;
    pcf_write_command(data_t, bytes); 
}
void lcd_write(char cmd) 
{
    char upper, lower; // upper, lower nibbles
    uint8_t data_t[4];
    int bytes = 4;;
    upper = (cmd&0xf0); // upper nibble
    lower = ( (cmd<<4) & 0xf0); // lower nibble
    // strobe commands by togglign enable line, rs = 0 to signal a command
    data_t[0] = upper | 0x0d; // en = 1 , rs = 1
    data_t[1] = upper | 0x09; // en = 0 , rs = 1
    data_t[2] = lower | 0x0d; 
    data_t[3] = lower | 0x09; 
    pcf_write_command(data_t, bytes); 
}
void lcd_send_string (char *str)
{
	while (*str) lcd_write (*str++);
}
void setup(void) {
    // startup sound
    for (int j = 70; j > 0; j--) {
        P3_6 ^= 1;	
        delay(j);
        delay(5);
    }

    for (int j = 20; j > 0; j--) {
        for (int i = 50; i>0; i--) {
            P3_6 ^= 1;
            delay(j);
        }
    }
    for (int j = 50; j > 0; j--) {
        P3_6 ^= 1;	
        delay(10);
    }
    for (int j = 50; j > 0; j--) {
        P3_6 ^= 1;	
        delay(5);
    }

    // Strings to LCD
    delay(55);
    lcd_cmd(0x02); 
    delay(55);
    lcd_send_string("Hello World");
    delay(55);
    lcd_cmd(0xc3);
    delay(55);
    lcd_send_string("Bob is Great!");
    delay(55);
}

void main(void)
{
    delay(1000);
    setup();
    delay(1000);
    int v = 0;
    int mode = SEG;
    int count = 0; // counter for LEDs
    bool left = true;
	while (1) {
        // holds binary representation of button presses
        unsigned char pos = 0;
        // start i2c instruction
        start();
        cmdout(0x41); // read device at address 000
        // ACK from slave
        SCL = 1;
        delay(1);
        if (SDA) {  // NACK
            // do something
        } else { // ACK
            // do something
        }
        SCL = 0;

        
        for (int i = 7; i >= 0; i--) {
            SCL = 0;
            SCL = 1;
            delay(1);
            if (SDA) { // default i2c is active low
                // do something
            }
            else {
                pos |= (1<<i); // bit pos mask
                // do something
            }
            delay(10);
        }

        // ACK from master
        SDA = 1;
        delay(1);
        SCL = 1;
        delay(1);
        SCL = 0;

        stop(); // only read buttons from p0-p7

        start();
        cmdout(0x40); // write 

        // ACK from slave
        SCL = 1;
        delay(1);
        if (SDA) {  // NACK
            // do something
        } else { // SDA low is ACK
            // do something
        }
        SCL = 0;

        cmdout(0xFF); // write to ports

        // ACK from slave
        SCL = 1;
        delay(1);
        if (SDA) {  // NACK
            // do something
        } else { // SDA low is ACK
            // do something
        }
        SCL = 0;
        cmdout(~pos); // p10-p17
        // ACK from slave
        SCL = 1;
        delay(1);
        if (SDA) {  // NACK
            // do something
        } else { // SDA low is ACK
            // do something
        }
        SCL = 0;
        stop();

        // toggles between LED features
        if (K1 == 0) {
            delay(5);
            while(!K1) {
            }
            mode++;
            if (mode == 4) {
                mode = 0;
            }
            v = 0;
            count = 0;
            P1 = 0xFF;
        }
        if (mode == SEG) {
            int t = 0; // var for button timer
            // 0 = button press
            // K3-K4 increments/decrements counter on 7-segment display
            if (K3 == 0) {
                delay(5);
                if (K3 == 0) {
                    v++;
                    while(!K3) {  // Display current value if button is held
                        display_seg(v);
                    }
                    if (v > 9999) {
                        v = 0;
                    }
                }
            }
            if (K4 == 0) {
                delay(5);
                if (K4 == 0) {
                    v--;
                    while(!K4) {
                        display_seg(v);
                    }
                    if (v < 0) {
                        v = 9999;
                    }
                }
            }
            display_seg(v);
        }

        if (mode == COUNT) {

            P1 = ~count;
            count++;
            if (count == 255) {
                count = 0;
            }
            delay_led(100);
        }
        if (mode == CYCLON) {
            // 3 = 00111111 on LEDS; take inverse = 11000000 on LEDS
            // right to left on the cylon, then  left to right on the next loops
            if (count == 6) {
                left = !left;
                count = 0;
            }

            if (left) {
                P1 = ~ (3<<count) ;
                delay_led(100);
            } else {
                P1 = ~ (192>>count) ;
                delay_led(100);
            }
            count++;
        }
        if (mode == DUAL) {
            if (count == 6) {
                count = 0;
            }
            P1 = ~(3<<count | 192>>count);
            count++;
            delay_led(100);
        }
	}
}