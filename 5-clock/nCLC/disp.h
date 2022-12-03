// disp.h - interface for driver for the display/TM1650
#ifndef _DISP_H_
#define _DISP_H_

// Constants for dots in disp_show()
#define DISP_DOTNO     0
#define DISP_DOT1      0 // Not connected
#define DISP_DOT2      2
#define DISP_DOT3      1 // connected to Digit 1
#define DISP_DOT4      0 // Not connected
#define DISP_DOTCOLON1 4 // connected to Digit 3
#define DISP_DOTCOLON2 8 // connected to Digit 4
#define DISP_DOTCOLON  ( DISP_DOTCOLON1 | DISP_DOTCOLON2 ) // Colon1 and Colon2
#define DISP_ALL_DOTS  ( DISP_DOT2 | DISP_DOT3 | DISP_DOTCOLON1 | DISP_DOTCOLON2 ) // all dots

void disp_brightness_set(int brightness);       // Sets brightness level 1..8
int  disp_brightness_get();                     // Gets brightness level
void disp_power_set(int power);                 // Sets power 0 (off) or 1 (on)
int  disp_power_get();                          // Gets power level

void disp_init();                               // Initializes display (prints error to Serial)
void disp_show(const char * s, uint8_t dots=0); // Puts (first 4 chars of) `s` (padded with spaces) on display, using flags in `dots` for P

#endif
