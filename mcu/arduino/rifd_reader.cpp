/* 
This code is intended for Arduino Uno (or Freenove starter board).
Using the MC522 RFID reader, it reads the UID of a card and send the data to the server application for validation.
The data is sent over the serial port (USB) to the computer running the server application.
*/

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 9;   // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;   // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

// Define the LED pin
const int ledPin = 8;

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522

  pinMode(ledPin, OUTPUT);  // Initialize the LED pin as an output
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(50);

    return;
  }

  // Dump UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Blink LED if card read
  for (int i = 0; i < 3; i++) {
      digitalWrite(ledPin, HIGH);  // Turn the LED on
      delay(1000);                  // Wait for duration
      digitalWrite(ledPin, LOW);   // Turn the LED off
      delay(1000);                  // Wait for duration
    }

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}
