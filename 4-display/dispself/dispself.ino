// dispself.ino - tests for the TM1650 display controller - own driver
// board: Generic ESP8285 module

#include <Wire.h>

#define SCL_PIN 12
#define SDA_PIN 13

// Control parameters
int brightness = 0;
int mode78 = 0;
int power = 1;

void set_control() {
  Serial.printf("brightness=%d, mode78=%d, power=%d\n",brightness,mode78,power);
  int val = (brightness << 4) | (mode78<<3) | (power<<0);
  Wire.beginTransmission(0x24); // register 0x48 DIG1CTRL
  Wire.write(val);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  Serial.printf("\n\ndispself.ino\n");
  Serial.printf("press b(rightnes), m(ode78), or p(ower)\n\n");
  Wire.begin(SDA_PIN,SCL_PIN);

  Wire.beginTransmission(0x34); // register 0x68 DIG1DATA
  //           PGFEDCBA
  Wire.write(0b00000110);       // segments for '1'
  Wire.endTransmission();

  Wire.beginTransmission(0x35); // register 0x6A DIG2DATA
  //           PGFEDCBA
  Wire.write(0b01001111);       // segments for '3'
  Wire.endTransmission();

  Wire.beginTransmission(0x36); // register 0x6C DIG3DATA
  //           PGFEDCBA
  Wire.write(0b11101101);       // segments for '5.'
  Wire.endTransmission();

  Wire.beginTransmission(0x37); // register 0x6E DIG3DATA
  //           PGFEDCBA
  Wire.write(0b10000111);       // segments for '7.'
  Wire.endTransmission();

  set_control();
}

void loop() {
  int ch=Serial.read();
  if( ch=='b' ) {
    brightness = (brightness+1)%8;
    set_control();
  } else if( ch=='m' ) {
    mode78 = !mode78;
    set_control();
  } else if( ch=='p' ) {
    power = !power;
    set_control();
  }
}
