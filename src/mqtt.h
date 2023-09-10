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
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
char msg1[MSG_BUFFER_SIZE];
char msg3[MSG_BUFFER_SIZE];
int value = 0;



#define state_topic "/home/boy_on/state" 
#define inTopic "/home/#"
// 
// MQTT topics
const char* CURRENT_TEMP_GET_BOY = "/home/boy_on/current-temperature/get";
const char* CURRENT_TEMP_SET_BOY = "/home/boy_on/current-temperature/set";
const char* TEMP_SETPOINT_GET_BOY = "/home/boy_on/setpoint-temperature/get";
const char* TEMP_SETPOINT_SET_BOY = "/home/boy_on/setpoint-temperature/set";
const char* MODE_GET_TOPIC_BOY = "/home/boy_on/mode/get";
const char* MODE_SET_TOPIC_BOY = "/home/boy_on/mode/set";
const char* TEMP_BOILER_GET_TOPIC = "/home/boy_on/boiler-temperature/get";
const char* TEMP_BOILER_TARGET_GET_TOPIC = "/home/boy_on/boiler-target-temperature/get";
const char* TEMP_BOILER_GIS = "/home/boy_on/gis-temperature/get";

const char* CURRENT_TEMP_GET_HEAT = "/home/heat_on/current-temperature/get";
const char* CURRENT_TEMP_SET_HEAT = "/home/heat_on/current-temperature/set";
const char* TEMP_SETPOINT_GET_HEAT = "/home/heat_on/setpoint-temperature/get";
const char* TEMP_SETPOINT_SET_HEAT = "/home/heat_on/setpoint-temperature/set";
const char* MODE_GET_TOPIC_HEAT = "/home/heat_on/mode/get";
const char* MODE_SET_TOPIC_HEAT = "/home/heat_on/mode/set";
const char* TEMP_HEAT_GET_TOPIC = "/home/heat_on/boiler-temperature/get";
const char* TEMP_HEAT_OFF_TOPIC = "/home/heat_on/boiler-temperature/off";
const char* TEMP_HEAT_TARGET_GET_TOPIC = "/home/heat_on/boiler-target-temperature/get";
const char* TIME_HEAT_CIKL = "/home/heat_on/setpoint-time/cikl";
const char* TIME_HEAT_IMPULS = "/home/heat_on/setpoint-time/impuls";

float temp_boy = 45.6,
      temp_heat = 55.6;
const String modeSetTopic(MODE_SET_TOPIC_BOY);
float temp = 0.00;
float hum = 0.00;
float tempDiff = 0.2;
float humDiff = 1.0;
int sensorTakt = 2000; //alle 2 Sekunden wird der Sensor ausgelesen
// long lastMsg = 0;
// char msg[50];
// int value = 0;
unsigned long oldmillis;
WiFiClient espClient;
PubSubClient client(espClient);
// long lastMsg = 0;  
                                                    // вызывается когда приходят данные от брокера
void callback(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Message arrived on topic: ");
  // Serial.print(topic);
  // Serial.println(". Message: ");

payload[length] = '\0';
String message = (char*)payload;

   if (strcmp(topic, MODE_SET_TOPIC_BOY) == 0){
     if (message == "heat"){
    eeprom.heat = true;
    // client.publish(MODE_GET_TOPIC_BOY, "heat");
    Serial.println("Heat: "+String(eeprom.heat));
   }else if (message == "off")
   {
    eeprom.heat = false;
   
    // client.publish(MODE_GET_TOPIC_BOY, "off");
    Serial.println("Heat: "+String(eeprom.heat));
    
   }
     
  }else if (strcmp(topic, TEMP_SETPOINT_SET_BOY) == 0){
    float temp_boy = message.toFloat();
    eeprom.temp_u = temp_boy;
    Serial.println("Ustavka Boyler: "+String(eeprom.temp_u));
  }
  // 

  if (strcmp(topic, MODE_SET_TOPIC_HEAT) == 0){
     if (message == "heat"){
    eeprom.heat_otop = true;
    // client.publish(MODE_GET_TOPIC_HEAT, "heat");
    Serial.println("Heat_otop: "+String(eeprom.heat_otop));
   }else if (message == "off")
   {
    eeprom.heat_otop = false;
    
    // client.publish(MODE_GET_TOPIC_HEAT, "off");
    Serial.println("Heat: "+String(eeprom.heat_otop));
 

   } }else if (strcmp(topic, TEMP_SETPOINT_SET_HEAT) == 0){
    float temp_heat = message.toFloat();
    eeprom.temp_u_b = temp_heat;
    Serial.println("Ustavka Otopl: "+String(eeprom.temp_u_b));

  }else if (strcmp(topic, TEMP_BOILER_GIS) == 0){
    float temp_gis = message.toFloat();
    eeprom.gis_boy = temp_gis;
    Serial.println("GIS Otopl: "+String(eeprom.gis_boy));

  }else if (strcmp(topic, TIME_HEAT_CIKL) == 0){
    int time_cikl = message.toInt();
    eeprom.per_off = time_cikl;
    Serial.println("cikl Otopl: "+String(eeprom.per_off));

  }else if (strcmp(topic, TIME_HEAT_IMPULS) == 0){
    int time_imp = message.toInt();
    eeprom.per_on = time_imp;
    Serial.println("impuls Otopl: "+String(eeprom.per_on));

  }else if (strcmp(topic, TEMP_HEAT_OFF_TOPIC) == 0){
    float temp_off = message.toFloat();
    eeprom.temp_off_otop = temp_off;
    Serial.println("temp_off Otopl: "+String(eeprom.temp_off_otop));
  }

}

                                               // подключение к mqtt брокеру            
void reconnect() {                                                      

 if (WiFi.status() != WL_CONNECTED){

ConnectWIFI();
  }
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // String clientId = "ESP8266Client-";
    // clientId += String(random(0xffff), HEX);
    // Attempt to connect
    client.setServer(mqtt_server, 1883);
    if (client.connect("arduinoClient", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
    //   delay(5000);
    }
  
}

// 

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
 temp_heat = bat.Update_f();
 temp_boy =  boyler.Update_f();
    
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

// 
snprintf (msg, MSG_BUFFER_SIZE, "%ld", rssi);
    client.publish("outTopic/rssi",msg);
// snprintf (msg, MSG_BUFFER_SIZE, "%ld", String(eeprom.temp_u_b).c_str());
dtostrf(eeprom.temp_u,2,2,msg);
    client.publish(TEMP_SETPOINT_GET_BOY, msg);
dtostrf(eeprom.temp_u_b,2,2,msg);
    client.publish(TEMP_SETPOINT_GET_HEAT, msg);
dtostrf(eeprom.temp_off_otop,2,2,msg);    
     client.publish(TEMP_HEAT_OFF_TOPIC, msg);
dtostrf(eeprom.gis_boy,2,2,msg);    
     client.publish(TEMP_BOILER_GIS, msg);

snprintf (msg, MSG_BUFFER_SIZE, "%ld", eeprom.per_on);
// dtostrf(eeprom.per_on,2,2,msg);    
     client.publish(TIME_HEAT_IMPULS, msg);
snprintf (msg, MSG_BUFFER_SIZE, "%ld", eeprom.per_off);
// dtostrf(eeprom.per_on,2,2,msg);    
     client.publish(TIME_HEAT_CIKL, msg);
dtostrf(temp_boy,2,2,msg);
    client.publish(CURRENT_TEMP_GET_BOY, msg);
dtostrf(temp_heat,2,2,msg);
    client.publish(CURRENT_TEMP_GET_HEAT, msg);



if (eeprom.heat)
{
  client.publish(MODE_GET_TOPIC_BOY, "heat");
  /* code */
} else {
client.publish(MODE_GET_TOPIC_BOY, "off");
}
if (eeprom.heat_otop)
{
  client.publish(MODE_GET_TOPIC_HEAT, "heat");
  /* code */
}
else{
  client.publish(MODE_GET_TOPIC_HEAT, "off");
}


}
