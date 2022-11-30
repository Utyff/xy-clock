# PCB analysis of the XY-Clock

Reverse engineering the display control of the XY-Clock.


## TM1650

First, I checked the pinning of the TM1650.
The datasheet is Chinese (I could only find bot-translated English versions).
However, determining the pin-out from the [Chinese datasheet](https://datasheet.lcsc.com/lcsc/1810281208_TM-Shenzhen-Titan-Micro-Elec-TM1650_C44444.pdf)
is not hard.

In the photo below, the DIGx pins of the TM1650 are labeled in black.
Those are the common cathode pins of the 4 digits.
In red, find the segment pins A, B, C, D, E, F, G and P (decimal point).
In blue the other connections (power and I2C).

![pcb-disp](pcb-disp.png)


## Display

Above and below the battery, the pins of the display are located.
I could not find (a datasheet of) the display.
It has marking `8401AW`; my guess is that
 - `8_0` indicates the size: 0.80 inch.
 - `4` indicates the number of digits.
 - I don't know what the `1` means, it is maybe an indication of which dots
   (decimal point, colon) are wired.
 - The `A` means common cathode (a `B` would mean common anode).
 - `W` is for white (O for orange, Y for Yellow, G for green, B for blue and somehow S for red).

For example [ELT5361BY](http://www.yitenuo.com/product/display/three/ELT-5361.html) is
a 0.56 inch 3-digit Yellow  common Anode display from Etnel LED Technology.

I believe the internal schematics of the display, and the pinning is as follows.
DIG1, DIG2, DIG3 and DIG4 are the common cathode pins of the four digits.
The segment pins are labeled A, B, C, D, E, F, G and P (decimal point).

Note P (decimal point) segments
- P segments of first and fourth number are not connected.
- P segment of second number connected to DIG2
- P segment of third number connected to DIG1
- The two dots of the colon connected to DIG3 and DIG4 as P segment.


![display internal](disp-intl.png)


## Connection

The TM1650 and the display are made for each other. Both have 4 common cathodes (DIG1..DIG4) and 8 segments (A-G,P).
The XY-Clock has the correct connection of the display segments to the TM1650. All segments are connected in the correct order, as they should.


## Datasheet

The [Chinese datasheet](https://datasheet.lcsc.com/lcsc/1810281208_TM-Shenzhen-Titan-Micro-Elec-TM1650_C44444.pdf)
gives us the following key points (focus on display; ignoring support for key scan):

 - The device is "semi" I2C. 
   It is I2C in the sense that it requires a _start_ and _stop_ condition, and in between those, bytes are sent. 
   It is not I2C because the device itself has no I2C address, it needs to be on a bus by its own.
   It borrows from I2C that it has registers that the host (ESP8266) should write to,
   and those registers have a (one byte) address.
   
 - There is one system _control_ register and four _data_ registers.
   
 - The control register is at location 48H (0x48), it determines whether the display is on or off, 
   whether a 7 or 8 segment display is attached, and what brightness to use.
   
   ![control](TM1650-control.png)

   Observe that 000 is tagged as brightness 8 in the datasheet - think of this as (1)000.

   I believe the 7/8 segments settings changes the timing of the digit/segment control:
   mode 7 has one segment less (P is always on).
   
   A write of 0b0101_0001 to 0x48 configures the display to have brightness 5, 8 segments and power On.
   
 - Since the device does not have an address itself, the example configuration needs a transaction
   of the form `START 48 51 STOP`. In the Arduino Wire API, we need to begin a transaction 
   by passing the _device_ address. For TM1650 we simply pass the _register_ address instead.
   However, a device address is 7 bits and it is appended with a 0 for write. 
   So we need to pass 0x24 to get 0x48 on the line.
   
   ```C++
   Wire.beginTransmission(0x24); // register 0x48
   Wire.write(0x51);
   Wire.endTransmission();
   ```

 - Fortunately all TM1650 registers are _even_
   so the trick to replace the device address
   by the register address always works.

 - For displaying content there is a _data_ register per digit.
 
 - The data registers for digits 1 to 4 are at address 0x68, 0x6A, 0x6C and 0x6E (this is overly verbose in the datasheet).

   ![data](TM1650-data.png)
   
   This table does show an important property, namely that bits 0, 1, 2, 3, 4, 5, 6, and 7
   map to segments A, B, C, D, E, F, G, respectively P - as one would expect.
 
 - To write a `0` digit to the display, we need to power segments A, B, C, D, E, and F, so that
   is the (blue) bits 0b1111_1100 or 0xF3:

   ```C++
   Wire.beginTransmission(0x34); // register 0x68
   Wire.write(0xF3);
   Wire.endTransmission();
   ```

(end)
  
