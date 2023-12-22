#include "Arduino.h"
#include <MFRC522.h>
#include <EEPROM.h>

MFRC522 rfid(5, 16); 

int eeprom_counter = 0 ; 
int x = 0 ; 

String get_UID() ; 
void add_new_user(String UID, String Username) ; 
void print_eeprom() ; 
void get_eeprom_counter() ; 
void set_eeprom_counter() ; 
void empty_eeprom() ; 


void setup()
{
  Serial.begin(115200) ; 
  EEPROM.begin(512);
  SPI.begin(); 
  rfid.PCD_Init();

  get_eeprom_counter() ;
  Serial.print("EEPROM Counter:   ") ; 
  Serial.println(eeprom_counter) ; 

  // empty_eeprom() ;  
}
void loop()
{
  x++ ; 
  String temp = get_UID() ; 
  if (temp != "NOCARD")
  {
    Serial.println(temp) ; 
    add_new_user(temp, "erfan") ; 
  }
  if (x == 200)
  {
    print_eeprom() ;
    Serial.println(" done. ") ; 
    vTaskDelay(3000) ;  x = 0 ; 
  }
}

String get_UID()
{
  String tag = "";
  if ( ! rfid.PICC_IsNewCardPresent())
  {
    return "NOCARD";
  }
  if (rfid.PICC_ReadCardSerial()) 
  {
    for (byte i = 0; i < 4; i++) 
    {
      tag += rfid.uid.uidByte[i];
    }
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  return tag;
}

void add_new_user(String UID, String Username)
{
  // adding UID
  for (int i=0 ; i<15 ; i++)
  {
    if (i <= UID.length())
    {
      EEPROM.write(eeprom_counter, (char)UID[i]) ; 
    }
    else 
    {
      EEPROM.write(eeprom_counter, '@') ; 
    }
    eeprom_counter++ ; 
  }
  EEPROM.commit() ; 
  vTaskDelay(50) ; 

  // adding Username
  for (int i=0 ; i<16 ; i++)
  {
    if (i <= Username.length())
    {
      EEPROM.write(eeprom_counter, (char)Username[i]) ; 
    }
    else 
    {
      EEPROM.write(eeprom_counter, '#') ; 
    }
    eeprom_counter++ ; 
  }
  EEPROM.commit() ; 
  vTaskDelay(50) ; 

  // adding default initial condition of the user
  EEPROM.write(eeprom_counter, 0) ; 
  eeprom_counter++ ; 
  EEPROM.commit() ; 
  vTaskDelay(50) ; 

  set_eeprom_counter() ; 

  Serial.println("User Added Successfully.") ; 
  
}

void print_eeprom()
{
  for (int i=0 ; i<15 ; i++)
  {
    for (int j=32*i ; j<32*i+15 ; j++)
    {
      char t_char = (char)EEPROM.read(j) ; 
      if (t_char != '@')
      {
        Serial.print(t_char) ; 
      }
      vTaskDelay(5) ; 
    }

    Serial.print("   ") ; 

    for (int j=32*i+15 ; j<32*i+31 ; j++)
    {
      char t_char = (char)EEPROM.read(j) ; 
      if (t_char != '#')
      {
        Serial.print(t_char) ; 
      }
      vTaskDelay(5) ; 
    }

    Serial.print("   ") ;

    Serial.println( (char)EEPROM.read(32*i+31) )  ; 
    vTaskDelay(5) ; 
  }
}

void get_eeprom_counter()
{
  uint8_t a = EEPROM.read(510) ; 
  uint8_t b = EEPROM.read(511) ; 
  eeprom_counter = (int) (a+b) ; 
}

void set_eeprom_counter()
{
  if (eeprom_counter<=255)
  {
    EEPROM.write(510, (uint8_t) eeprom_counter) ; 
    EEPROM.write(511, 0) ; 
  }
  else 
  {
    EEPROM.write(510, 255) ; 
    EEPROM.write(511, (uint8_t) (eeprom_counter-255)) ; 
  }
}

void empty_eeprom()
{
  for (int i=0 ; i<512 ; i++)
  {
    EEPROM.write(i, 0) ; 
    vTaskDelay(1) ; 
  }
  EEPROM.commit() ; 
  vTaskDelay(50) ; 
}

