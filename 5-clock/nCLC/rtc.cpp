// rtc.cpp - driver for the rtc DS1307
#include <Arduino.h>
#include <Wire.h>
#include <ErriezDS1307.h> // https://github.com/Erriez/ErriezDS1307
#include "rtc.h"

#define CHK(err) {                      \
    if (!err) {                         \
        Serial.print(F("Failure: "));   \
        Serial.print((strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__));     \
        Serial.print(F(":"));           \
        Serial.println(__LINE__);       \
        Serial.flush();                 \
        noInterrupts();                 \
        while (1)                       \
            ;                           \
    }                                   \
}

// The I2C connections
#define SCL_PIN 12
#define SDA_PIN 13

// The number of registers in the DS1307
#define  DS_REGISTERS_NUM 0x40
#define  ID_STR_ADDR      0x8
#define  ID_STR_LENGTH    8
static const uint8_t *const ID_STR = (const uint8_t*)"nCLC_id";

// RTC object
ErriezDS1307 rtc;


bool rtcInit() {
    Serial.println("rtc : init");

    // Initialize I2C
    Wire.begin(SDA_PIN, SCL_PIN);
    // Wire.setClock(100000);

    // Initialize RTC
    CHK(rtc.begin() == true);

    return true;
}

bool rtcCheck() {
    uint8_t buf[8];

    if (!rtc.isRunning()) {
        Serial.println("Check fail: RTC not running.");
        return false;
    }

    CHK(rtc.readBuffer(ID_STR_ADDR, buf, ID_STR_LENGTH));
    Serial.printf("read ID: %02x %02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
    for (int i = 0; i < ID_STR_LENGTH; i++) {
        if (ID_STR[i] != buf[i]) {
            Serial.printf("Wrong char at i=%d  ID char: %02x - read char: %02x\n", i, ID_STR[i], buf[i]);
            Serial.println("Check fail: ID string not match.");
            return false;
        }
    }
    Serial.println("ID matched. Check passed");
    return true;
}

bool rtcGet(struct tm *dt) {
    CHK(rtc.read(dt));
    return true;
}

bool rtcSet(const struct tm *dt) {
    CHK(rtc.clockEnable(true));
    CHK(rtc.writeBuffer(ID_STR_ADDR, (void *) ID_STR, ID_STR_LENGTH));
    CHK(rtc.write(dt));

    return true;
}
