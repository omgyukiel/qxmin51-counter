• Using a make file: make && stcgal -p COM8 -P stc89 -a LedBlank.ihx
• Using sdcc: sdcc LedBlink.c && stcgal -p COM8 -P stc89 LedBlink.ihx

Steps:
Compile C file using sdcc
ex: sdcc LedBlink.c

ISP Flash the ihx file using stcgal
ex:
stcgal -p COM8 -P stc89 LedBlink.ihx
-p sets serial port of MCU, -P sets the protocol f, LedBlink.ihx is the path to the code image

