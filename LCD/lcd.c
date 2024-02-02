#include <8051.h>
#include <stdint.h>
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
// sends 8 bit command through serial
void cmdout(char cmd) 
{
    for (char i = 0; i < 8; i++) {
        if (cmd & MSK) {
            SDA = 1;
        }
        else {
            SDA = 0;
        }
        delay(1);
        SCL = 1;
        cmd = cmd << 1;  // clock data into sda
        SCL = 0;
    }
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
        delay(1);
        if (SDA) {  // NACK
            P1 = 0x00;
            delay(100);
            P1 = 0xFF;
            // stop();
        } else { // SDA low is ACK
            P1 = 0xFF;
            delay(1);
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

// write bytes amount of commands to i2c
void pcf_write_command(uint8_t *commands, int bytes) 
{
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
   r/w is only set high when reading from lcd */


//sends command to lcd through i2c 
// note 1 byte = P7 P6 P5 P4 P3 P2 P1 P0  == D7 D6 D5 D4 LED E R/W RS
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

// writes data to lcd through i2c, same as command but with register select set to high
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

// void lcd_write(char cmd) 
// {
//     char upper, lower; // upper, lower nibbles
//     uint8_t data_t[4];
//     int bytes = 4;;
//     upper = (cmd&0xf0) >> 4; // upper nibble
//     lower = ( (cmd<<4) & 0xf0) >> 4; // lower nibble
//     // strobe commands by togglign enable line, rs = 0 to signal a command
//     data_t[0] = upper | 0xD0; // en = 1 , rs = 1
//     data_t[1] = upper | 0x90; // en = 0 , rs = 1
//     data_t[2] = lower | 0xD0; 
//     data_t[3] = lower | 0x90; 
//     pcf_write_command(data_t, bytes); 
// }

void lcd_clear (void)
{
	lcd_cmd (0x01);
	delay(300);
}

void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    lcd_cmd (col);
}

void lcd_send_string (char *str)
{
	while (*str) lcd_write (*str++);
}

void lcd_init(void) {
    // initializes 4 bit mode
    // init source is in espi2clcd_2.jpeg within resources
    // https://controllerstech.com/i2c-in-esp32-esp-idf-lcd-1602/#info_box
    // note 1 byte = P7 P6 P5 P4 P3 P2 P1 P0  == D7 D6 D5 D4 LED E R/W RS

    // delay(2300); // delay >40ms
    // lcd_cmd(0x30);
    // delay(600); // >4.1msd
    // lcd_cmd(0x30);
	// delay(5);  // >100us
    // lcd_cmd(0x30);
	// delay(600);
    // lcd_cmd(0x20); // 4 bit mode
	// delay(600);

    // // dislay initialisation
    	lcd_cmd (0x02); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters) //28 is old
delay(55);
	lcd_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters) //28 is old
	delay(55);
    lcd_cmd(0x0c); //Display on/off control --> D=0,C=0, B=0  ---> display off
	// lcd_cmd(0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	delay(55);
	lcd_cmd(0x06);  // clear display
    	// lcd_cmd(0x06);  // clear display

	delay(55);
    	lcd_cmd(0x01);  // clear display

	delay(55);
	// lcd_cmd(0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	// delay(55);
	// lcd_cmd(0x0F); //Display on/off control --> D = 1, C and B = 0. (Cursor and brink, last two bits)
	// delay(55);
    

}

void main(void)  
{  
    // for (int j = 70; j > 0; j--) {
    //     P3_6 ^= 1;	
    //     delay(j);
    //     delay(5);
    // }

    // for (int j = 20; j > 0; j--) {
    //     for (int i = 50; i>0; i--) {
    //         P3_6 ^= 1;
    //         delay(j);
    //     }
    // }
    // for (int j = 50; j > 0; j--) {
    //     P3_6 ^= 1;	
    //     delay(10);
    // }
    // for (int j = 50; j > 0; j--) {
    //     P3_6 ^= 1;	
    //     delay(5);
    // }
    lcd_init();
    delay(55);
    lcd_cmd(0x80); // set cursor to start
	delay(55);
    // lcd_put_cur(0,0);
    // lcd_write('B');
    for (char i = 'A'; i <= 'P' ; i++) {
        lcd_write(i);
        delay(5000000);
    }

    lcd_cmd(0xC0); // set cursor to start
    // for (char i = 'Q'; i <= 'Z' ; i++) {
    //     lcd_write(i);
    //     delay(500);
    // }
    // lcd_write('A');
    // lcd_write('B');
    // lcd_write('C');

    delay(55);
    //  lcd_write('D');
    // delay(55);
    // uint8_t test 
    // pcf_write_command(data_t, bytes); 
    // lcd_cmd(0x02);
    // lcd_put_cur(1,3);

    // lcd_write('A');
    // lcd_clear();
    // delay(55);
    // delay_led(1000);
    // lcd_put_cur(1,3);
    // delay(55);
    // lcd_cmd(0x02);
    // delay(55);
    // delay(55);
    // lcd_write("4");
    lcd_send_string("Hello dudes");
    lcd_send_string("Kenny is Great!");


    while(1) {

    }

}

