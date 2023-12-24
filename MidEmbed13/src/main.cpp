#include "Arduino.h"
#include <server.h>
#include <RFID.h>

void check_rfid(void*){
  while(true){
    if(!rfid_flag){
      String temp = get_UID();
      if(temp != "NOCARD"){
        change_user_state(temp);
      }
    }
    vTaskDelay(10);
  }
}
void system_init(void*){
  Serial.begin(115200) ; 
  EEPROM.begin(512);
  SPI.begin(); 
  rfid.PCD_Init();
  WiFi.begin("Predator" , "123456mr");
  while(!WiFi.isConnected());
  Serial.println(WiFi.localIP());
  server_setup();
  read_eeprom_counter() ;
  empty_eeprom() ; 
  xTaskCreatePinnedToCore(
    check_rfid,
    "check rfid",
    5120,
    NULL,
    10,
    NULL,
    APP_CPU_NUM
  );
  while(true){
    vTaskDelay(10);
  }
}
void setup()
{
  xTaskCreatePinnedToCore(
    system_init,
    "system init",
    10240,
    NULL,
    10,
    NULL,
    PRO_CPU_NUM
  );
  vTaskDelete(NULL);
  // Serial.begin(115200) ; 
  // EEPROM.begin(512);
  // SPI.begin(); 
  // rfid.PCD_Init();
  // WiFi.begin("Predator" , "123456mr");
  // while(!WiFi.isConnected());
  // Serial.println(WiFi.localIP());
  // server_setup();
  // read_eeprom_counter() ;
  // //Serial.print("EEPROM Counter:   ") ; 
  // //Serial.println(eeprom_counter) ; 

  // empty_eeprom() ;  
}
void loop()
{
  //x++ ; 
  // String temp = get_UID() ;
  // Serial.println(temp); 
  // if (temp != "NOCARD")
  // {
  //   Serial.println(temp) ; 
  //   // add_new_user(temp, "erfan") ; 
  //   // UID_exist(temp) ; 
  //   // change_user_state(temp) ; 
  //   //delete_user(temp) ; 
  // }
  // if (x == 200)
  // {
  //   print_eeprom() ;
  //   Serial.println(" done. ") ; 
  //   vTaskDelay(3000) ;  x = 0 ; 
  // }
  vTaskDelay(10);
}
