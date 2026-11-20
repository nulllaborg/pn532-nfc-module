/*
 * Read a MIFARE Classic card UID with the PN532 over I2C.
 * Hardware: Arduino Uno and PN532 in I2C mode.
 * Driver: bundled Adafruit PN532 driver.
 */

#include <Wire.h>
#include <Adafruit_PN532.h>

// I2C wiring: connect IRQ to D2 and RST to D3 when using those signals.
#define PN532_IRQ (2)
#define PN532_RESET (3)

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup()
{
  Serial.begin(115200);
  Serial.println("PN532 M1 Card Reader");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    Serial.println("PN532 module not detected. Check the wiring and DIP switches.");
    while (1)
      ;
  }

  Serial.print("PN532 chip found. Firmware version: ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.SAMConfig();
  Serial.println("Waiting for M1 card...");
}

void loop()
{
  uint8_t uid[7] = {0};
  uint8_t uidLength;

  // Look for an ISO/IEC 14443-A card, such as MIFARE Classic.
  boolean success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A,
                                            uid, &uidLength, 100);

  if (success)
  {
    Serial.print("Card UID: ");
    for (uint8_t i = 0; i < uidLength; i++)
    {
      if (uid[i] < 0x10)
        Serial.print("0");
      Serial.print(uid[i], HEX);
      if (i < uidLength - 1)
        Serial.print(" ");
    }
    Serial.println();
    delay(1000); // Pause before polling again to limit repeated reads.
  }
}