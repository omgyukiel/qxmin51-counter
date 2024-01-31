#include <8051.h>

#define SDA P0_0
#define SCL P0_1
#define INT P0_2
#define DBG P0_3
#define MSK 0x80

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

void main(void) 
{  
    //  TMOD |= 0x20;		
	// TH1 = 0xFD;
	// TR1 = 1;
	// TI = 1;			// enable serial port interrupt

    while(1) {


        start();
        cmdout(0x41); // read device at 000
        // ACK from slave
        SCL = 1;
        delay(1);
        if (SDA) {  // NACK
            // P1 = 0xFF;
            // delay(10);
            // stop();
        } else { // SDA low is ACK
            // P1 = 0x00;
            // delay(10);
            // stop();
        }
        SCL = 0;
        // holds binary representation of button presses
        unsigned char pos = 0;
        for (int i = 7; i >= 0; i--) {
            SCL = 0;
            SCL = 1;
            delay(1);
            if (SDA) { // read port
                // P1 = ~i; // 1;
            }
            else {
                pos |= (1<<i); // bit pos mask
                P1 = ~pos;
                // pos[i] = 1;
                delay(100);
                P1 = 0xFF;
                // if (i == 4) {
                //     P1 = 0x00;
                //     pos[i] = 1;
                //     delay(100);
                //     P1 = 0xFF;
                // }
                
            }
            delay(10);
            // SCL = 0;
        }
        // cmdin(); // p7-p0

        // ACK from master
        SDA = 1;
        delay(1);
        SCL = 1;
        delay(1);
        SCL = 0;

        stop(); // only read buttons

        // cmdin(); // p17-p10
        // for (int i = 7; i >= 0; i--) {
        //     SCL = 0;
        //     SCL = 1;
        //     delay(1);
        //     if (SDA) { // read port
        //         // P1 = ~i; // 1;
        //     }
        //     else {
        //         P1 = 0x00;
        //     }
        //     delay(10);
        //     // SCL = 0;
        // }
        // // ACK from master
        // SDA = 1;  
        // delay(1);
        // SCL = 1;
        // delay(1);
        // SCL = 0;


        // stop();

// write all
        start();
        cmdout(0x40); // write 

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
            // stop();
        }
        SCL = 0;

        cmdout(0xFF); // write to ports

        // ACK from slave
        SCL = 1;
        delay(1);
        if (SDA) {  // NACK
            P1 = 0X00;
            delay(100);
            P1 = 0xFF;
            delay(100);
            // stop();
        } else { // SDA low is ACK
            P1 = 0xFF;
            delay(100);
            // stop();
        }
        SCL = 0;

        // cmdout(~bitpos);
        cmdout(~pos); // p10-p17
        // ACK from slave
        SCL = 1;
        delay(1);
        if (SDA) {  // NACK
            // P1 = 0x00;
            delay(10);
            // stop();
        } else { // SDA low is ACK
            P1 = 0xFF;
            delay(100);
            // stop();
        }
        SCL = 0;


        stop();
    }



}

