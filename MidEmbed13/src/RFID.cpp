#include <RFID.h>

int eeprom_counter = 0 ; 
int x = 0 ; 
MFRC522 rfid(5, 16);
MFRC522::MIFARE_Key key;

void rfid_setup(){
    //begin SPI connection
  SPI.begin();
  rfid.PCD_AntennaOn();
  rfid.PCD_Init();

  // Just wait some seconds...
  vTaskDelay(4);
  // Prepare the security key for the read and write functions.
  // Normally it is 0xFFFFFFFFFFFF
  // Note: 6 comes from MF_KEY_SIZE in MFRC522.h
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF; //keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
  }
  //Serial.println("RFID is Ready!");
  //delay(100);
  return;
}
int check_master(){
    String temp = get_UID();
    if(temp == "NOCARD")
      return NOCARD;
    if(temp == "89254112113")
        return MASTER;
    else
        return OTHER;
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
  // if (UID_exist(UID))
  // {
  //   Serial.println(" User Already Exists! ") ; 
  //   return ; 
  // }
  // if(name_exist(Username)){
  //   Serial.println(" User Already Exists! ") ; 
  //   return ;     
  // }
  // Serial.println(" after checks ") ; 
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
  //Serial.println(" after adding uid ") ; 
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
  EEPROM.write(eeprom_counter, '0') ; 
  eeprom_counter++ ; 
  EEPROM.commit() ; 
  vTaskDelay(50) ; 

  write_eeprom_counter() ; 

  //Serial.println("User Added Successfully.") ; 
  
}

String** print_eeprom()
{
    String** result;
    result = new String*[15];
    for (int i=0 ; i<eeprom_counter/32 ; i++)
    {
        result[i] = new String[3];
        Serial.println("new list");
        for (int j=32*i ; j<32*i+15 ; j++)
        {
            char t_char = (char)EEPROM.read(j) ; 
            if (t_char != '@')
            {
                //Serial.print(t_char) ;
                result[i][0] += t_char;
                //result+= t_char; 
            }
            vTaskDelay(5) ; 
        }

        //Serial.print("   ") ; 
        //result += "\0";
        for (int j=32*i+15 ; j<32*i+31 ; j++)
        {
            char t_char = (char)EEPROM.read(j) ; 
            if (t_char != '#')
            {
            //Serial.print(t_char) ;
                result[i][1] += t_char;
                //result+= t_char;  
            }
            vTaskDelay(5) ; 
        }
        //result += "\0";
        //Serial.print("   ") ;
        result[i][2] = (char)EEPROM.read(32*i+31);
        //result += (char)EEPROM.read(32*i+31);
        //Serial.println( EEPROM.read(32*i+31) )  ; 
        vTaskDelay(5) ; 
    }
    return result;
}

void read_eeprom_counter()
{
  uint8_t a = EEPROM.read(510) ; 
  uint8_t b = EEPROM.read(511) ; 
  eeprom_counter = (int) (a+b) ; 
}

void write_eeprom_counter()
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

bool UID_exist(String UID)
{
  int dismatch_count = 0 ; 
  for (int i=0 ; i<15 ; i++)
  {
    for (int j=0 ; j<UID.length() ; j++)
    {
      if (UID[j] != (char)EEPROM.read(32*i+j))
      {
        dismatch_count++ ; 
        break ; 
      }
    }
  }
  Serial.print("dismatch: ") ; 
  Serial.println(dismatch_count) ; 
  if (dismatch_count == 15)
  {
    Serial.println("false uid") ; 
    return false ; 
  }
  else 
  {
    Serial.println("true") ; 
    return true ; 
  }
}
bool name_exist(String name){
    int dismatch_count = 0 ; 
    for (int i=0 ; i<15 ; i++)
    {
        for (int j=0 ; j< name.length() ; j++)
        {
            if (name[j] != (char)EEPROM.read(32*i+15+j))
            {
              dismatch_count++ ; 
              break ; 
            }
        }
    }
    Serial.print("dismatch: ") ; 
    Serial.println(dismatch_count) ; 
    if (dismatch_count == 15)
    {
        Serial.println("false name") ; 
        return false ; 
    }
    else 
    {
        Serial.println("true") ; 
        return true ; 
    }    
}
void change_user_state(String UID)
{
  int match_count = 0 ; 
  for (int i=0 ; i<15 ; i++)
  {
    for (int j=0 ; j<UID.length() ; j++)
    {
      if (UID[j] == (char)EEPROM.read(32*i+j))
      {
        match_count++ ; 
      }
    }
    Serial.print("match count: ") ; 
    Serial.println(match_count) ; 
    if (match_count == UID.length())
    {
      if (EEPROM.read(32*i+31) == '0')
      {
        Serial.println("changed to 1") ; 
        EEPROM.write(32*i+31, '1') ; 
      }
      else 
      {
        Serial.println("changed to 0") ; 
        EEPROM.write(32*i+31, '0') ; 
      }
      EEPROM.commit() ; 
      match_count = 0 ; 
      break ; 
    }
    match_count = 0 ; 
  }
}

void delete_user_uid(String UID)
{
  int match_count = 0 ; 
  for (int i=0 ; i<15 ; i++)
  {
    for (int j=0 ; j<UID.length() ; j++)
    {
      if (UID[j] == (char)EEPROM.read(32*i+j))
      {
        match_count++ ; 
      }
    }
    Serial.print("match count for delete: ") ; 
    Serial.println(match_count) ; 
    if (match_count == UID.length())
    {
      if (eeprom_counter >= 32)
      {
        eeprom_counter -= 32 ; 
      }
      Serial.print("eeprom counter = ") ; 
      Serial.println(eeprom_counter) ; 
      write_eeprom_counter() ; 
      Serial.println("after write eeprom counter") ; 
      for (int j=32*i ; j<32*i+32 ; j++)
      {
        EEPROM.write(j, 0) ; 
      }
      EEPROM.commit() ; 
      Serial.println("after make it free") ; 
      vTaskDelay(100) ; 
      shift_eeprom( 32*(i+1) ) ; 
      Serial.println("after updating it") ; 
      break ; 
    }
    match_count = 0 ; 
  }
}
void delete_user_name(String name)
{
  int match_count = 0 ; 
  for (int i=0 ; i<15 ; i++)
  {
    for (int j=0 ; j<name.length() ; j++)
    {
      if (name[j] == (char)EEPROM.read(32*i+15+j))
      {
        match_count++ ; 
      }
    }
    Serial.print("match count for delete: ") ; 
    Serial.println(match_count) ; 
    if (match_count == name.length())
    {
      Serial.print("eeprom counter = ") ; 
      Serial.println(eeprom_counter) ; 
      Serial.println("after write eeprom counter") ; 
      for (int j=32*i ; j<32*i+32 ; j++)
      {
        EEPROM.write(j, 0) ; 
      }
      EEPROM.commit() ; 
      Serial.println("after make it free") ; 
      vTaskDelay(100) ; 
      shift_eeprom( 32*(i+1) ) ; 
      Serial.println("after updating it") ; 
      break ; 
    }
    match_count = 0 ; 
  }
}

void shift_eeprom(int statrting_point)
{
  for (int i=statrting_point ; i<eeprom_counter ; i++)
  {
    uint8_t a = EEPROM.read(i) ; 
    EEPROM.write(i-32, a) ; 
    vTaskDelay(1) ; 
  }
  EEPROM.commit() ; 
  if (eeprom_counter >= 32)
  {
    eeprom_counter -= 32 ; 
  }
  write_eeprom_counter() ; 
}

