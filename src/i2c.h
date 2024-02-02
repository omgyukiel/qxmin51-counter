#ifndef I2C_H 
#define I2C_H
void delay(unsigned int t);
void delay_led(unsigned int i);
void start(void); 
void cmdout(unsigned char cmd);
void stop(void);
#endif
