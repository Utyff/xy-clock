#include <Wire.h>
#include <ErriezDS1307.h> // https://github.com/Erriez/ErriezDS1307

// Create RTC object
ErriezDS1307 rtc;

#define SCL_PIN     12
#define SDA_PIN     13
#define Success     true
#define Failure     false

#define CHK(err) {                      \
    if ((err) == Success) {             \
        while (0);                      \
    } else {                            \
        Serial.print(F("Failure: "));   \
        Serial.print((strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__));     \
        Serial.print(F(":"));           \
        Serial.println(__LINE__);       \
        Serial.flush();                 \
        noInterrupts();                 \
        while (1);                      \
    }                                   \
}

// A test epoch "Sunday, September 6, 2020 18:20:30"
#define EPOCH_TEST      1599416430UL


void setup() {
    struct tm dtw;
    struct tm dtr;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t mday;
    uint8_t mon;
    uint16_t year;
    uint8_t wday;

    // Initialize serial port
    delay(500);
    Serial.begin(115200);
    while (!Serial);
    Serial.println(F("\nDS1307 begin test\n"));

    rtcInit();
    rtcCheck();

    CHK(rtc.read(&dtr));
    Serial.printf("\nFirst read: %d %02d %02d %d - %02d:%02d:%02d\n", dtr.tm_year, dtr.tm_mon, dtr.tm_mday, dtr.tm_wday, dtr.tm_hour, dtr.tm_min, dtr.tm_sec);

    // Test setEpoch()
    CHK(rtc.setEpoch(EPOCH_TEST) == Success);
    delay(1500);

    // Verify epoch struct tm
    CHK(rtc.read(&dtr) == Success);
    Serial.printf("\nEPOCH read: : %d %02d %02d %d - %02d:%02d:%02d\n", dtr.tm_year, dtr.tm_mon, dtr.tm_mday, dtr.tm_wday, dtr.tm_hour, dtr.tm_min, dtr.tm_sec);
//    CHK(dtr.tm_sec == 31);
    CHK(dtr.tm_min == 20);
    CHK(dtr.tm_hour == 18);
    CHK(dtr.tm_mday == 6);
    CHK(dtr.tm_mon == 8);
    CHK(dtr.tm_year == 120);
    CHK(dtr.tm_wday == 0);

    // Verify getEpoch()
    CHK(rtc.getEpoch() == (EPOCH_TEST + 1));

    // Test write()
    Serial.println("Set RTC");
    dtw.tm_hour = 12;
    dtw.tm_min = 34;
    dtw.tm_sec = 56;
    dtw.tm_mday = 29;
    dtw.tm_mon = 1; // 0=January
    dtw.tm_year = 2020-1900;
    dtw.tm_wday = 6; // 0=Sunday
    CHK(rtcSet(&dtw));

    // Test read()
    Serial.println("Get RTC");
    CHK(rtcGet(&dtr));
    CHK(dtw.tm_sec == dtr.tm_sec);
    CHK(dtw.tm_min == dtr.tm_min);
    CHK(dtw.tm_hour == dtr.tm_hour);
    CHK(dtw.tm_mday == dtr.tm_mday);
    CHK(dtw.tm_mon == dtr.tm_mon);
    CHK(dtw.tm_year == dtr.tm_year);
    CHK(dtw.tm_wday == dtr.tm_wday);

    // Test setTime()
    CHK(rtc.setTime(12, 34, 56) == Success);
    delay(1500);
    CHK(rtc.getTime(&hour, &min, &sec) == Success);
    CHK((hour == 12) && (min == 34) && (sec == 57));

    // Test setDateTime()   13:45:09  31 December 2019  Tuesday
    CHK(rtc.setDateTime(13, 45, 9,  31, 12, 2019,  2) == Success);
    delay(1500);

    // Test getDateTime()
    CHK(rtc.getDateTime(&hour, &min, &sec, &mday, &mon, &year, &wday) == Success);
    CHK(hour == 13);
    CHK(min == 45);
    CHK(sec == 10);
    CHK(mday == 31);
    CHK(mon == 12);
    CHK(year == 2019);
    CHK(wday == 2);

    // Verify setDateTime() with read()
    CHK(rtc.read(&dtr) == Success);
    CHK(dtr.tm_sec == 10);
    CHK(dtr.tm_min == 45);
    CHK(dtr.tm_hour == 13);
    CHK(dtr.tm_mday == 31);
    CHK(dtr.tm_mon == 11);    // Month 0..11
    CHK(dtr.tm_year == (2019 - 1900)); // Year - 1900
    CHK(dtr.tm_wday == 2);    // 0=Sun, 1=Mon, 2=Tue

    // Completed
    Serial.println(F("Test passed"));
}

void loop() {
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t mday;
    uint8_t mon;
    uint16_t year;
    uint8_t wday;

    CHK(rtc.getDateTime(&hour, &min, &sec, &mday, &mon, &year, &wday) == Success);
    Serial.printf("Time: %d %02d %02d %d - %02d:%02d:%02d\n", year, mon, mday, wday, hour, min, sec);

  delay(1000);
}

#define  DS_REGISTERS_NUM 0x40
// Read and print all DS1307 registers
void readAll() {
  uint8_t buf[DS_REGISTERS_NUM];

  // for(int i=8; i<DS_REGISTERS_NUM; i++) {
  //   CHK(rtc.writeRegister(i, i));
  // }

  CHK(rtc.readBuffer(0, buf, DS_REGISTERS_NUM));
  Serial.printf("\n DS1307 registers:");

  for(int i = 0; i<DS_REGISTERS_NUM; i++){
    if(i%8 == 0) {
      Serial.printf("\n %02x  ", i);
    }
    Serial.printf(" %02x", buf[i]);
  }
  Serial.printf("\n");
}

bool rtcInit() {
    // Initialize I2C
    Wire.begin(SDA_PIN,SCL_PIN);
    // Wire.setClock(100000);

    // Initialize RTC
    Serial.println("DS1307 Initialize.");
    CHK(rtc.begin() == true);
    Serial.println("DS1307 Init successful.");

    readAll();

    return true;
}

#define ID_STR_LENGTH 8
const uint8_t ID_STR[ID_STR_LENGTH] = "nCLC_id";

bool rtcCheck() {
  uint8_t buf[8];

  if (!rtc.isRunning()) {
    Serial.println("Check fail: RTC not running.");
    return false;
  }

  CHK(rtc.readBuffer(0x8, buf, ID_STR_LENGTH));
  Serial.printf("read ID: %02x %02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
  for(int i=0; i<ID_STR_LENGTH; i++) {
    if(ID_STR[i]!=buf[i]) {
      Serial.printf("Wrong char at i=%d  ID char: %02x - read char: %02x\n", i, ID_STR[i], buf[i]);
      Serial.println("Check fail: ID string not match.");
      return false;
    }
  }
  Serial.printf("ID matched. Check passed");
  return true;
}

bool rtcGet(struct tm *dt) {
  CHK(rtc.read(dt));
  return true;
}

bool rtcSet(const struct tm *dt) {
  CHK(rtc.clockEnable(true));
  CHK(rtc.writeBuffer(0x8, (void*)ID_STR, ID_STR_LENGTH));
  CHK(rtc.write(dt));

  return true;
}
