#include <Wire.h>
#include "disp.h"
#include "rtc.h"
#include <ErriezDS1307.h> // https://github.com/Erriez/ErriezDS1307


#define CHK(err) {                      \
    if (!err) {                         \
        Serial.print(F("Failure: "));   \
        Serial.print((strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)); \
        Serial.print(F(":"));           \
        Serial.println(__LINE__);       \
        Serial.flush();                 \
        noInterrupts();                 \
        while (1);                      \
    }                                   \
}


void setup() {
    struct tm dtw;
    struct tm dtr;

    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial);
    Serial.println(F("\nDS1307 begin test\n"));

    // Identify oursleves, regardless if we go into config mode or the real app
    disp_init();
    disp_power_set(1);
    disp_show("nClC");

    rtcInit();
    rtcCheck();

    CHK(rtcGet(&dtr));
    Serial.printf("\nFirst read: %d %02d %02d %d - %02d:%02d:%02d\n", dtr.tm_year,
                  dtr.tm_mon, dtr.tm_mday, dtr.tm_wday, dtr.tm_hour, dtr.tm_min, dtr.tm_sec);

    // Set time for test
    Serial.println("Set RTC");
    dtw.tm_hour = 12;
    dtw.tm_min = 34;
    dtw.tm_sec = 56;
    dtw.tm_mday = 29;
    dtw.tm_mon = 1; // 0=January
    dtw.tm_year = 2020 - 1900;
    dtw.tm_wday = 6; // 0=Sunday
    CHK(rtcSet(&dtw));

    CHK(rtcGet(&dtr));
    Serial.printf("Time: %d %02d %02d %d - %02d:%02d:%02d\n", dtr.tm_year,
                  dtr.tm_mon, dtr.tm_mday, dtr.tm_wday, dtr.tm_hour, dtr.tm_min, dtr.tm_sec);

    // Completed
    Serial.println(F("Init passed\n"));
    delay(1000);
}

void loop() {
    struct tm dtr;

    CHK(rtcGet(&dtr));
    Serial.printf("Time: %d %02d %02d %d - %02d:%02d:%02d\n", dtr.tm_year,
                  dtr.tm_mon, dtr.tm_mday, dtr.tm_wday, dtr.tm_hour, dtr.tm_min, dtr.tm_sec);

    char now[5];
    sprintf(now,"%2d%02d", dtr.tm_min, dtr.tm_sec);
    disp_show(now,DISP_DOTCOLON);

    delay(1000);
}
