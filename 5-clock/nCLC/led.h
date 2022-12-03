// led.h - interface to control the LED on the XY-Clock board
#ifndef _LED_H_
#define _LED_H_

#define LED1_PIN 0
#define LED2_PIN 2

void led_init();
void led_set(int on);
void led2_set(int on);
int  led_get();
int  led2_get();
void led_on();
void led2_on();
void led_off();
void led2_off();

#endif
