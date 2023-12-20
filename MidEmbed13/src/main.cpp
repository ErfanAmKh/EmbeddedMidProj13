#include "Arduino.h"
#include <MFRC522.h>
#include <EEPROM.h>

MFRC522 rfid(5, 16); 

void setup()
{
  Serial.begin(115200) ; 
  EEPROM.begin(512);
  SPI.begin(); 
  rfid.PCD_Init();
}
void loop()
{
  String tag = "";
  if (rfid.PICC_IsNewCardPresent())
  {
    if (rfid.PICC_ReadCardSerial()) 
    {
      for (byte i = 0; i < 4; i++) 
      {
        tag += rfid.uid.uidByte[i];
      }
      Serial.println(tag) ; 
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
  vTaskDelay(1);
}