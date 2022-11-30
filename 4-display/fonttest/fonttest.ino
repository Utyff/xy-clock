// fonttest.ino - tests for the TM1650 display controller - own driver - segment remap and font
// board: Generic ESP8285 module

#include <Wire.h>

static const uint8_t disp_font[0x80] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // padding for 0x0_
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // padding for 0x1_
  //pgfedcba
  0b00000000, // 20 spc
  0b00101000, // 21 !
  0b00100010, // 22 "
  0b01100011, // 23 #
  0b01001001, // 24 $
  0b00100100, // 25 %
  0b01111110, // 26 &
  0b00000010, // 27 '
  0b00111001, // 28 (
  0b00001111, // 29 )
  0b00000001, // 2A *
  0b01000010, // 2B +
  0b00001100, // 2C ,
  0b01000000, // 2D -
  0b00010000, // 2E .
  0b01010010, // 2F /
  //pgfedcba
  0b00111111, // 30 0
  0b00000110, // 31 1
  0b01011011, // 32 2
  0b01001111, // 33 3
  0b01100110, // 34 4
  0b01101101, // 35 5
  0b01111101, // 36 6
  0b00000111, // 37 7
  0b01111111, // 38 8
  0b01101111, // 39 9
  0b00001001, // 3A :
  0b00001010, // 3B ;
  0b01011000, // 3C <
  0b01001000, // 3D =
  0b01001100, // 3E >
  0b01001011, // 3F ?
  //pgfedcba
  0b00111011, // 40 @
  0b01110111, // 41 A
  0b01111100, // 42 B
  0b00111001, // 43 C
  0b01011110, // 44 D
  0b01111001, // 45 E
  0b01110001, // 46 F
  0b00111101, // 47 G
  0b01110110, // 48 H
  0b00110000, // 49 I
  0b00011110, // 4A J
  0b01110101, // 4B K
  0b00111100, // 4C L
  0b01010101, // 4D M
  0b00110111, // 4E N
  0b00111111, // 4F O
  //pgfedcba
  0b01110011, // 50 P
  0b01101011, // 51 Q
  0b00110011, // 52 R
  0b01101101, // 53 S
  0b01111000, // 54 T
  0b00111110, // 55 U
  0b01110010, // 56 V
  0b01101010, // 57 W
  0b00110110, // 58 X
  0b01101110, // 59 Y
  0b01011011, // 5A Z
  0b00111001, // 5B [
  0b01100100, // 5C \ (\ shall not end line in C)
  0b00001111, // 5D ]
  0b00100011, // 5E ^
  0b00001000, // 5F _
  //pgfedcba
  0b00100000, // 60 `
  0b01011111, // 61 a
  0b01111100, // 62 b
  0b01011000, // 63 c
  0b01011110, // 64 d
  0b01111011, // 65 e
  0b01110001, // 66 f
  0b01101111, // 67 g
  0b01110100, // 68 h
  0b00000101, // 69 i
  0b00001101, // 6A j
  0b01110101, // 6B k
  0b00111000, // 6C l
  0b01010101, // 6D m
  0b01010100, // 6E n
  0b01011100, // 6F o
  //pgfedcba
  0b01110011, // 70 p
  0b01100111, // 71 q
  0b01010000, // 72 r
  0b01101101, // 73 s
  0b01111000, // 74 t
  0b00011100, // 75 u
  0b01110010, // 76 v
  0b01101010, // 77 w
  0b00010100, // 78 x
  0b00101110, // 79 y
  0b01011011, // 7A z
  0b01000110, // 7B {
  0b00000110, // 7C |
  0b01110000, // 7D }
  0b01000001, // 7E ~
  0b01011101  // 7F del
};

#define SCL_PIN 12
#define SDA_PIN 13


void disp_init() {
  Wire.begin(SDA_PIN,SCL_PIN);
  int brightness = 8; // max
  int mode7 = 0;      // 8 segments
  int power = 1;      // on
  int val = ((brightness%8) << 4) | (mode7<<3) | (power<<0);
  Wire.beginTransmission(0x24); // register 0x48 DIG1CTRL
  Wire.write(val);
  int result = Wire.endTransmission();
  if( result==0 ) Serial.printf("disp: init\n");
  else Serial.printf("disp: init ERROR %d\n",result);
}

void disp_show(const char * s) {
  for(int i=0; i<4; i++ ) {
    // Lookup for char *s which segments to enable. *s is truncated to 7 bits, bit 8 is for P
    uint8_t segments = (disp_font[*s & 0x7F]) | ( *s & 0x80 );
    // TM1650 is not wired 1-1 to display, so remap segments
    Wire.beginTransmission(0x34+i);
    Wire.write(segments);
    Wire.endTransmission();
    if( *s ) s++; // next char unless at end
  }
}

const char * message = "   !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f   ";
const char * m;

void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nfonttest.ino\n");
  disp_init();
  m = message;
}

void loop() {
  disp_show(m);
  if( *m ) m++; else m=message;
  delay(500);
}
