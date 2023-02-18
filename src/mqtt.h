#include <PubSubClient.h>
// #include "json.h"
#include <ArduinoJson.h> 
const size_t capacity_post = JSON_ARRAY_SIZE(7) + JSON_OBJECT_SIZE(1) + 7*JSON_OBJECT_SIZE(2);
DynamicJsonDocument doc_post(capacity_post);
//MQTT Server
const char *mqtt_server = "greenhouse.net.ua"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT
const char *mqtt_user = "mqtt"; // Логин от сервер
const char *mqtt_pass = "qwerty"; // Пароль от сервера


unsigned long oldmillis;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;  
                                                    // вызывается когда приходят данные от брокера
void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
 // Serial.print(topic);                              // отправляем в монитор порта название топика                     
 // Serial.print("] ");
  for (int i = 0; i < length; i++) {                // отправляем данные из топика
    //Serial.print((char)payload[i]);
  }
  //Serial.println();  
}
                                               // подключение к mqtt брокеру            
void reconnect() {                                                      
unsigned long ms=millis();
   if(ms - oldmillis > 1000) {
    oldmillis = ms;                                                       // подключаемся, в client.connect передаем ID, логин и пасс
    if (client.connect("arduinoClient", mqtt_user, mqtt_pass)) {
     
      
    } 
   }
}

void setupMqtt() {  
  
  client.setServer(mqtt_server, mqtt_port);           // указываем адрес брокера и порт
 // client.setCallback(callback); 
  // указываем функцию которая вызывается когда приходят данные от брокера
}
void loopMQtt() {
  JsonArray tags = doc_post.createNestedArray("tags");
JsonObject tags_0 = tags.createNestedObject();
// 
tags_0["koll"] = kollektor.Update_f();
tags_0["RSS"] = rssi;
JsonObject tags_1 = tags.createNestedObject();
// tags_1["id"] = 15;
tags_1["boy"] = boyler.Update_f();

JsonObject tags_2 = tags.createNestedObject();
// tags_2["id"] = 16;
tags_2["bat"] = bat.Update_f();
char output[256];
size_t n = serializeJson(doc_post, output);
terminal.println(output);

  char msg[6];                                            // забераем температуру и конвертируем её в char
  float tmpin = kollektor.Update_f();
  dtostrf(tmpin, 4, 2, msg);

  char msg1[6];                                            // забераем температуру и конвертируем её в char
  float tmp_out = bat.Update_f();
  dtostrf(tmp_out, 4, 2, msg1);

  char msg2[6];                                            // забераем температуру и конвертируем её в char
  float lux_in = rssi;
  dtostrf(lux_in, 4, 2, msg2);

  char msg3[6];                                            // забераем температуру и конвертируем её в char
  float temp_boy = boyler.Update_f();
  dtostrf(temp_boy, 4, 2, msg3);
 
  



   
  if (!client.connected()) {                             // проверяем подключение к брокеру
    reconnect();                                            // еще бы проверить подкючение к wifi...
  }
  client.loop();

  long now = millis();                                   // каждые 10 секунд
  if (now - lastMsg > 1000) {
    lastMsg = now; 
    client.publish("/home/temp_koll", msg);                     // пишем в топик 
    client.publish("/home/temp_bat", msg1);
    client.publish("/home/WiFi", msg2);
    client.publish("/home/temp_boy", msg3);
    client.publish("/home/temp_json", output,n);
    doc_post.clear();
    

   
  }
}
