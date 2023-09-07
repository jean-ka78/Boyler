#include <PubSubClient.h>
// #include "json.h"
#include <ArduinoJson.h> 
const size_t capacity_post = JSON_ARRAY_SIZE(7) + JSON_OBJECT_SIZE(1) + 7*JSON_OBJECT_SIZE(2);
DynamicJsonDocument doc_post(capacity_post);
//MQTT Server
const char *mqtt_server = "greenhouse.net.ua"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
const char *mqtt_user = "mqtt_boyler"; // Логин от сервер
const char *mqtt_pass = "qwerty"; // Пароль от сервера

#define state_topic "/home/boy_on/state" 
#define inTopic "/home/boy_on"
// #define state_kol "/home/kol_on/state"
// #define inKol "/home/kol_on"

float temp = 0.00;
float hum = 0.00;
float tempDiff = 0.2;
float humDiff = 1.0;
int sensorTakt = 2000; //alle 2 Sekunden wird der Sensor ausgelesen
// long lastMsg = 0;
char msg[50];
int value = 0;
unsigned long oldmillis;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;  
                                                    // вызывается когда приходят данные от брокера
void callback(char* topic, byte* message, unsigned int length) {
  // terminal.print("Message arrived on topic: ");
  // terminal.print(topic);
  // terminal.print(". Message: ");
message[length] = '\0';
  String messageTemp = (char*)message;
  // String messageTemp;
  // char messageTemp;
  // for (int i = 0; i < length; i++) {
  //   terminal.print((char)message[i]);
  //   messageTemp += (char)message[i];
  // }
  // terminal.println();


  if (String(topic) == inTopic) 
  // if (strcmp(topic,state_topic))
  {
    if(messageTemp == "ON"){
            eeprom.heat = 1;
      client.publish(state_topic, "ON");
      Blynk.virtualWrite(V5,eeprom.heat);
     
      delay(100);
    }
    if(messageTemp == "OFF"){
      
      client.publish(state_topic, "OFF"); 
      eeprom.heat = 0;
      Blynk.virtualWrite(V5,eeprom.heat);


      delay(100);
    }
   
    if(messageTemp == "KolON"){
            eeprom.heat_otop = 1;
      client.publish(state_topic, "KolON");
      Blynk.virtualWrite(V15,eeprom.heat_otop);
     
      delay(100);
    }
    if(messageTemp == "KolOFF"){
      
      client.publish(state_topic, "KolOFF"); 
            eeprom.heat_otop = 0;
      Blynk.virtualWrite(V15,eeprom.heat_otop);
      terminal.print(messageTemp);
      // delay(100);
      
    }
    return;
  }  else if (String(topic) ==  "/home/boy_on/ustavka/boy"){
    float temp_boy = messageTemp.toFloat();
    eeprom.temp_u = temp_boy;
    Blynk.virtualWrite(V4,eeprom.temp_u);
    terminal.println(eeprom.temp_u);
  }
  else {
      terminal.println("Not working");
  }
  }

  

                                               // подключение к mqtt брокеру            
void reconnect() {                                                      
unsigned long ms=millis();
   if(ms - oldmillis > 1000) {
    oldmillis = ms;                                                       // подключаемся, в client.connect передаем ID, логин и пасс
    if (client.connect("arduinoClient", mqtt_user, mqtt_pass)) {
     client.subscribe(inTopic);
    //  client.subscribe(inKol);
      
    } 
   }
}

void setupMqtt() {  
  
  client.setServer(mqtt_server, mqtt_port);           // указываем адрес брокера и порт
  client.setCallback(callback); 
  // указываем функцию которая вызывается когда приходят данные от брокера
}
void loopMQtt() {

   
  if (!client.connected()) {                             // проверяем подключение к брокеру
    reconnect();                                            // еще бы проверить подкючение к wifi...
  }
  client.loop();
   
    
}
bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
  (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}      
void getValues() {
    
    
JsonArray tags = doc_post.createNestedArray("tags");
JsonObject tags_0 = tags.createNestedObject();

tags_0["koll"] = kollektor.Update_f();
tags_0["RSS"] = rssi;
JsonObject tags_1 = tags.createNestedObject();
tags_1["boy"] = boyler.Update_f();
JsonObject tags_2 = tags.createNestedObject();
tags_2["bat"] = bat.Update_f();


char output[256];
size_t n = serializeJson(doc_post, output);
client.publish("/home/temp_json", output,n);
doc_post.clear();
}
