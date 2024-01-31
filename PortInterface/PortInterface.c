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
        SCL = 1;
        cmd = cmd << 1;  // clock data into sda
        SCL = 0;
    }
}

void cmdin(void) 
{
    for (unsigned char i = 0; i < 8; i++) {
        SCL = 0;
        SCL = 1;
        delay(1);
        if (SDA) { // read port
            P1 = ~i; // 1;
        }
        else {
            P1 = 0xFF;
        }
        delay(10000);
        SCL = 0;
    }
}


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


    //TODO: write to SDA and turn on LED
    //TODO: read a button press from SDA
    //TODO: read button press then turn on LED
    while(1) {
        // DBG = 1;
        delay(10000);
        start();
        cmdout(0x41); // read address 000
        // ACK from slave
        SCL = 1;
        delay(1);
        if (SDA) {  // NACK
            P1 = 0xFF;
            delay(10);
            // stop();
        } else { // SDA low is ACK
            P1 = 0x20;
            // delay(10);
            // stop();
        }
        SCL = 0;

        cmdin(); // p7-p0

        // ACK from master
        SDA = 1;
        delay(1);
        SCL = 1;
        delay(1);
        SCL = 0;

        cmdin(); // p17-p10
        // ACK from master
        SDA = 1;  
        delay(1);
        SCL = 1;
        delay(1);
        SCL = 0;


        stop();

// // write all
//         cmdout(0x40); // write 
//         // ACK from slave
//         SCL = 1;
//         delay(1);
//         if (SDA) {  // NACK
//             P1 = 0xFF;
//             delay(10);
//             // stop();
//         } else { // SDA low is ACK
//             P1 = 0xFF;
//             delay(100);
//             // stop();
//         }
//         SCL = 0;

//         cmdout(0x40); // p7-p0 1s on p6

//         // ACK from slave
//         SCL = 1;
//         delay(1);
//         if (SDA) {  // NACK
//             P1 = 0xFF;
//             delay(10);
//             // stop();
//         } else { // SDA low is ACK
//             P1 = 0x00;
//             delay(100);
//             // stop();
//         }
//         SCL = 0;

//         cmdout(0x00); // p10-p17
//         // ACK from slave
//         SCL = 1;
//         delay(1);
//         if (SDA) {  // NACK
//             P1 = 0xFF;
//             delay(10);
//             // stop();
//         } else { // SDA low is ACK
//             P1 = 0x00;
//             delay(100);
//             // stop();
//         }
//         SCL = 0;


//         stop();
//     }



}

