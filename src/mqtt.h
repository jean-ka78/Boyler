#include <PubSubClient.h>
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
  
  char msg[6];                                            // забераем температуру и конвертируем её в char
  float tmpin = T_koll;
  dtostrf(tmpin, 4, 2, msg);

  char msg1[6];                                            // забераем температуру и конвертируем её в char
  float tmp_out = T_bat;
  dtostrf(tmp_out, 4, 2, msg1);

  char msg2[6];                                            // забераем температуру и конвертируем её в char
  float lux_in = rssi;
  dtostrf(lux_in, 4, 2, msg2);
 
  


   
  if (!client.connected()) {                             // проверяем подключение к брокеру
    reconnect();                                            // еще бы проверить подкючение к wifi...
  }
  client.loop();

  long now = millis();                                   // каждые 10 секунд
  if (now - lastMsg > 5000) {
    lastMsg = now; 
    client.publish("Температура koll", msg);                     // пишем в топик 
    client.publish("Температура bat", msg1);
    client.publish("WiFi", msg2);
  }
}
