/*
  TapLock - RFID access control
  Course project

  Hardware
    Arduino UNO R3      KS0001
    RC522 RFID reader   KS0067   (3.3 V only)
    Micro servo 9g      KS0209
    Passive buzzer      KS0019

  Wiring
    RC522 VCC  -> 3.3V     RC522 RST  -> D9     RC522 SDA  -> D10
    RC522 GND  -> GND      RC522 MOSI -> D11    RC522 MISO -> D12
                           RC522 SCK  -> D13
    Servo PWM  -> D6       servo +  -> 5V       servo -  -> GND
    Buzzer S   -> D8       buzzer + -> 5V       buzzer - -> GND

  Every attempt is printed to the serial port as one CSV line:
    millis,uid,result
  Lines starting with # are status messages, not data.
*/

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

// Pins
const uint8_t PIN_RST    = 9;
const uint8_t PIN_SS     = 10;
const uint8_t PIN_SERVO  = 6;
const uint8_t PIN_BUZZER = 8;

// Settings
const int      ANGLE_LOCKED   = 0;      // servo angle when locked
const int      ANGLE_OPEN     = 90;     // servo angle when open
const uint32_t UNLOCK_TIME_MS = 5000;   // how long the lock stays open
const uint8_t  MAX_TRIES      = 3;      // wrong cards in a row before lockout
const uint32_t LOCKOUT_MS     = 30000;  // how long cards are ignored after that

// Authorised cards, one UID per line. Print a UID with read_uid.ino and paste it here.
// The two below are preset cards of the Wokwi simulator; replace them with the real cards.
const uint8_t UID_SIZE = 4;
const uint8_t AUTHORISED[][UID_SIZE] = {
  { 0x11, 0x22, 0x33, 0x44 },   // Wokwi green card
  { 0xC0, 0xFF, 0xEE, 0x99 }    // Wokwi key fob
};
const uint8_t CARD_COUNT = sizeof(AUTHORISED) / sizeof(AUTHORISED[0]);

MFRC522 rfid(PIN_SS, PIN_RST);
Servo   lockServo;

enum State { LOCKED, UNLOCKED, LOCKOUT };
State    state      = LOCKED;
uint32_t stateSince = 0;       // millis() when the current state started
uint8_t  wrongTries = 0;

// Every state change goes through here, and the servo follows the state:
// only UNLOCKED opens the lock.
void setState(State s) {
  state = s;
  stateSince = millis();
  lockServo.write(s == UNLOCKED ? ANGLE_OPEN : ANGLE_LOCKED);
}

bool isAuthorised(const uint8_t *uid, uint8_t size) {
  if (size != UID_SIZE) return false;   // 7-byte UIDs are never in the list
  for (uint8_t i = 0; i < CARD_COUNT; i++) {
    if (memcmp(uid, AUTHORISED[i], UID_SIZE) == 0) return true;
  }
  return false;
}

void logAttempt(const uint8_t *uid, uint8_t size, const char *result) {
  Serial.print(millis());
  Serial.print(',');
  for (uint8_t i = 0; i < size; i++) {
    if (uid[i] < 0x10) Serial.print('0');
    Serial.print(uid[i], HEX);
  }
  Serial.print(',');
  Serial.println(result);
}

void beepAccepted() { tone(PIN_BUZZER, 1200, 120); }   // short, high
void beepDenied()   { tone(PIN_BUZZER,  220, 500); }   // long, low

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  // Set the angle before attach(), otherwise the servo starts at 90 degrees
  // and the lock opens for a moment at power-up.
  lockServo.write(ANGLE_LOCKED);
  lockServo.attach(PIN_SERVO);
  pinMode(PIN_BUZZER, OUTPUT);

  // A missing or badly wired reader answers 0x00 or 0xFF instead of its version.
  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  if (version == 0x00 || version == 0xFF) {
    Serial.println(F("# RC522 not responding, check the wiring"));
  }

  Serial.println(F("millis,uid,result"));
  setState(LOCKED);
}

void loop() {
  uint32_t now = millis();

  if (state == UNLOCKED && now - stateSince >= UNLOCK_TIME_MS) {
    setState(LOCKED);
  }
  if (state == LOCKOUT && now - stateSince >= LOCKOUT_MS) {
    wrongTries = 0;
    setState(LOCKED);
  }
  if (state == LOCKOUT) return;   // cards are not read during lockout

  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial())   return;

  if (isAuthorised(rfid.uid.uidByte, rfid.uid.size)) {
    wrongTries = 0;
    logAttempt(rfid.uid.uidByte, rfid.uid.size, "GRANTED");
    beepAccepted();
    setState(UNLOCKED);
  } else {
    wrongTries++;
    logAttempt(rfid.uid.uidByte, rfid.uid.size, "DENIED");
    beepDenied();
    if (wrongTries >= MAX_TRIES) {
      logAttempt(rfid.uid.uidByte, rfid.uid.size, "LOCKOUT");
      setState(LOCKOUT);
    }
  }

  rfid.PICC_HaltA();        // the card stays silent until it is taken away
  rfid.PCD_StopCrypto1();
}
