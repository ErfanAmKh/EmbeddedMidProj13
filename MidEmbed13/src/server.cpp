#include "server.h"

bool rfid_flag = false;
AsyncWebServer server(80);
char message_html[] = R"rawliteral(<!DOCTYPE html>
      <html>
      <style>
        html {
          background-color: #000;
          color: #FFFFFF
      }
      </style>
      <script type="text/javascript">
        function message(){
            alert("User                                     ");
            window.location.replace("/personnel");
        }
      </script>

      <body onload="message()"></body></html>)rawliteral";

void handle_root(AsyncWebServerRequest *);
void handle_mainpage(AsyncWebServerRequest *);
void handle_personnel_mainpage(AsyncWebServerRequest *);
void handle_personnel_mainpage_security(AsyncWebServerRequest *);
void handle_personnel_info(AsyncWebServerRequest*);
void handle_personnel_add(AsyncWebServerRequest *);
void handle_personnel_delete(AsyncWebServerRequest *);
void handle_report(AsyncWebServerRequest *);
void handle_report_info(AsyncWebServerRequest *);

void server_setup(){
    server.on("/" , handle_root);
    server.on("/security" , handle_personnel_mainpage_security);
    server.on("/personnel" , handle_personnel_mainpage);
    server.on("/personnelinfo" , handle_personnel_info);
    server.on("/report" ,  handle_report);
    server.on("/reportinfo" ,  handle_report_info);
    server.on("/personneldelete" , handle_personnel_delete);
    server.on("/personneladd" , handle_personnel_add);
    server.begin();    
}
void handle_root(AsyncWebServerRequest *request){
    request->send_P(200, "text/html", root_html);
}
void handle_personnel_mainpage_security(AsyncWebServerRequest * request){
    rfid_flag = true;
    int64_t timeout = esp_timer_get_time() + 7000000;
    DynamicJsonDocument doc(1024);
    JsonArray user = doc.createNestedArray("SECURE");
    JsonObject member;
    String data;
    while(esp_timer_get_time() < timeout){
        //check for master card...
        int stat = check_master();
        if(stat == NOCARD)
            vTaskDelay(10);
        else if(stat == MASTER){
            member = user.createNestedObject();
            member["STATUS"] = "true"; 
            serializeJsonPretty(doc , Serial);
            serializeJson(doc, data);     
            rfid_flag = false;
            rfid_setup();
            request->send(200 , "text/plain" , data);   
            return;              
        }
        else
            break;
    }
    member = user.createNestedObject();
    member["STATUS"] = "false";    
    serializeJsonPretty(doc , Serial);
    serializeJson(doc, data);  
    rfid_flag = false;
    rfid_setup();
    request->send(200 , "text/plain" , data); 
}
void handle_personnel_mainpage(AsyncWebServerRequest * request){
    request->send_P(200, "text/html", personnel_html);    
}
void handle_personnel_info(AsyncWebServerRequest* request){
    int i = 0;
    String data,
        **temp;
    DynamicJsonDocument doc(4096);
    JsonArray user = doc.createNestedArray("USERS");
    temp = print_eeprom();
    Serial.println("after temp");
    for(int i=0;i < eeprom_counter/32;i++)
    {
        if(!temp[i][0].isEmpty()){
            // Serial.println((int)temp[i][0][0]);
            JsonObject member = user.createNestedObject();
            member["UID"] = (temp[i][0]);
            member["NAME"] = (temp[i][1]);
        }
        else
            break;
    }
    vTaskDelay(1);
    //serializeJsonPretty(doc , Serial);
    serializeJson(doc, data);
    for(int i=0;i< eeprom_counter/32;i++){
        delete[] temp[i];
    }
    delete[] temp;
    request->send(200 , "text/plain" , data);    
}
void handle_personnel_add(AsyncWebServerRequest * request){
    rfid_flag = true;
    String UID, name;
    name = request->arg("NAME");
    //Serial.println(name);
    if(name.isEmpty())
    {
        request->send_P(401, "text/plain", "name is empty");
        //Serial.println("error : user name is empty ");
        return;
    }
    else if(name.length() > 40)
    {
        request->send_P(401, "text/plain", "name is too long");
        //Serial.println("error : user name is too long ");
        return;
    }
    name.toLowerCase();
    // //check if user name exist
    if(name_exist(name))
    {
        request->send_P(401, "text/plain", "user name exists , try a different name");
        //Serial.println("error : user name exists ");
        return;
    }
    // rfid.setup();
    // //getting user UID
    String temp;
    while(true)
    {
        temp = get_UID();
        if(temp != "NOCARD")
        {
            UID = temp;
            if(UID_exist(UID)){
                request->send_P(401, "text/plain", "user UID exists , try a different card");
                //Serial.println("error : user UID exists ");
                return;                
            }
            else
                break;
        }
        vTaskDelay(10);
    }
    rfid_setup();
    add_new_user(UID,name);
    snprintf(&message_html[230], 22, "added successfully.  ");
    message_html[251] = ' ';
    rfid_flag = false;
    request->send_P(200, "text/html", message_html);
}
void handle_personnel_delete(AsyncWebServerRequest * request){
    rfid_flag = true;
    String name = request->arg("NAME");
    if(name.isEmpty())
    {
        request->send_P(401, "text/plain" , "User name is empty");
        //Serial.println("error : User name is empty");
        return;
    }
    else
        name.toLowerCase();
    //check if username exist
    if(!name_exist(name))
    {
        request->send_P(401, "text/plain" , "User name not exist");
        //Serial.println("error : User name not exist");
        return;
    }
    delete_user_name(name);
    snprintf(&message_html[230], 22, "removed successfully.");
    message_html[251] = ' ';
    rfid_flag = false;
    request->send_P(200 , "text/html" , message_html);
}
void handle_report(AsyncWebServerRequest * request){
    request->send_P(200, "text/html", report_html);
}
void handle_report_info(AsyncWebServerRequest * request){
    
    String data,
        **temp;
    DynamicJsonDocument doc(4096);
    JsonArray user = doc.createNestedArray("USERS");
    Serial.println("hello");
    temp = print_eeprom();
    for(int i=0;i < eeprom_counter/32;i++)
    {
        if(temp[i][2] == "1"){
            Serial.println(temp[i][1]);
            // Serial.println(temp[i][2]);
            JsonObject member = user.createNestedObject();
            member["UID"] = (temp[i][0]);
            member["NAME"] = (temp[i][1]);
        }
        else
            continue;
    }
    vTaskDelay(1);
    //serializeJsonPretty(doc , Serial);
    serializeJson(doc, data);
    //Serial.println(data);
    request->send(200 , "text/plain" , data);
}