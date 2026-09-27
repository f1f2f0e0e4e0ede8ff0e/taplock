/*
  read_uid - helper sketch for TapLock
  Prints the UID of every card that touches the reader, as a line that can be
  pasted straight into the AUTHORISED table of taplock.ino.
  Same wiring as taplock.ino. Serial monitor at 9600 baud.
*/

#include <SPI.h>
#include <MFRC522.h>

const uint8_t PIN_RST = 9;
const uint8_t PIN_SS  = 10;

MFRC522 rfid(PIN_SS, PIN_RST);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  if (version == 0x00 || version == 0xFF) {
    Serial.println(F("RC522 not responding, check the wiring"));
  } else {
    Serial.print(F("RC522 found, version 0x"));
    Serial.println(version, HEX);
  }
  Serial.println(F("Touch a card to the reader"));
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial())   return;

  // for example:  { 0x11, 0x22, 0x33, 0x44 },   // 4-byte UID
  Serial.print(F("{ "));
  for (uint8_t i = 0; i < rfid.uid.size; i++) {
    Serial.print(F("0x"));
    if (rfid.uid.uidByte[i] < 0x10) Serial.print('0');
    Serial.print(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) Serial.print(F(", "));
  }
  Serial.print(F(" },   // "));
  Serial.print(rfid.uid.size);
  Serial.println(F("-byte UID"));

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
