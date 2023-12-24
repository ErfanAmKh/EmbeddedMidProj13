#ifndef RFID
#define RFID

#include <MFRC522.h>
#include <EEPROM.h>

#define MASTER 0
#define NOCARD 1
#define OTHER 2
#define MASTERUID "89254112113"

void rfid_setup();
String get_UID() ; 
void add_new_user(String UID, String Username) ; 
String** print_eeprom() ; 
void read_eeprom_counter() ; 
void write_eeprom_counter() ; 
void empty_eeprom() ; 
bool UID_exist(String UID) ; 
bool name_exist(String name);
void change_user_state(String UID) ; 
void delete_user_uid(String UID) ; 
void delete_user_name(String name) ; 
void shift_eeprom(int statrting_point) ; 
int check_master();

#if !defined(_RFID)
    extern MFRC522 rfid;
    extern int eeprom_counter;
#endif

#endif