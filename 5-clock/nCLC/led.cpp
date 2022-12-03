// led.cpp - control the LED on the XY-Clock board
#include <Arduino.h>
#include "led.h"


void led_init() {
  led_off();
  led2_off();
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  Serial.printf("led : init\n");
}

void led_set(int on) {
  digitalWrite(LED1_PIN, !on ); // low active
}

void led2_set(int on) {
  digitalWrite(LED2_PIN, !on ); // low active
}

int led_get() {
  return !digitalRead(LED1_PIN); // low active
}

int led2_get() {
  return !digitalRead(LED2_PIN); // low active
}

void led_on() {
  led_set(1);
}

void led2_on() {
  led2_set(1);
}

void led_off() {
  led_set(0);
}

void led2_off() {
  led2_set(0);
}
