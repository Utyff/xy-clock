// led-key-test.ino - tests the LED and switches on the board
// board: Generic ESP8285 module

#define D1_PIN 0
#define D2_PIN 2

#define S1_PIN 9  // change pin for 8266
#define S2_PIN 10 // change pin for 8266
#define S3_PIN 16

#define getS1() (digitalRead(S1_PIN)==0) // low active
#define getS2() (digitalRead(S2_PIN)==0) // low active
#define getS3() (digitalRead(S3_PIN)==0) // low active

void setup() {
    Serial.begin(115200);
    Serial.println("\n\nled-key-test.ino\n\n");

    // configure LEDs (D1 D2)
    digitalWrite(D1_PIN, HIGH); // low active
    pinMode(D1_PIN, OUTPUT);
    digitalWrite(D2_PIN, HIGH); // low active
    pinMode(D2_PIN, OUTPUT);

    // Configure switches (S1, S2, S3)
    pinMode(S1_PIN, INPUT_PULLUP);
    pinMode(S2_PIN, INPUT_PULLUP);
    pinMode(S3_PIN, INPUT);
}

int count = 0;

void loop() {
    digitalWrite(D1_PIN, LOW); // low active
    digitalWrite(D2_PIN, LOW); // low active
    delay(200);
    digitalWrite(D1_PIN, HIGH); // low active
    digitalWrite(D2_PIN, HIGH); // low active
    delay(800);
    Serial.printf("%04d %d %d %d\n", count++, getS1(), getS2(), getS3());
}
