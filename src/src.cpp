#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include "GyverFilters.h"
#include "NTC.h"
#include <ModbusIP_ESP8266.h>


BlynkTimer timer;
// Первое ли это подключение к Серверу
bool isFirstConnection=true;
WidgetLED led1(V6), led2(V9), led3(V10), led4(V13), led5(V16);
WidgetTerminal terminal(V12);
// ID для таймеров Blynk
int IDt_reconnectBlynk; // ID таймера для перезагрузки
 
#include "link.h"
#include "st_enum.h"
#include "dubug.h"
#include "modbus.h"


int thermistorPin1 = 33;// Вход АЦП, выход делителя напряжения
int thermistorPin2 = 32;
int thermistorPin3 = 35;
const int relay = 21;
const int nasos_otop = 19;
int PIN_LOW = 22;
int PIN_HIGH = 23;
uint32_t tmr;
bool flag = HIGH;
float T_boyler, T_koll, T_bat;
long rssi;
unsigned long old_time, old_time1, old_time2, old_time3, timer4;
NTC kollektor(thermistorPin1);
NTC boyler(thermistorPin2);
NTC bat(thermistorPin3);
#include "heat_regul.h"
#include "obogrev.h"
#include "mqtt.h"
void temp_in()
{
   Blynk.virtualWrite(V0, kollektor.Update_f());
   Blynk.virtualWrite(V1, boyler.Update_f());
   Blynk.virtualWrite(V3, bat.Update_f());
   Blynk.virtualWrite(V11, rssi);
}

void regul()
{
bool relle;
relle = logic(eeprom.heat,T_boyler,T_koll,eeprom.temp_u, eeprom.gis_boy);
if (!relle)
{
  led1.off();
} else 
{
  led1.on();
}
digitalWrite(relay,relle);
}


void setup()
{ 
  // setup_mb();
  setupMqtt();
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
  pinMode(PIN_LOW, OUTPUT);
  pinMode(PIN_HIGH, OUTPUT);
  pinMode(nasos_otop, OUTPUT);
  EEPROM.begin(sizeof(st_Enum));
  EEPROM.put(0, eeprom);
// --------------------------------------
// Заводские настройки 
  eeprom.temp_u = 50;
  eeprom.temp_u_b = 50;
  eeprom.temp_off_otop = 35;
  eeprom.gis_boy = -5;
  eeprom.heat = true;
  eeprom.heat_otop = true;
  eeprom.per_on = 10;
  eeprom.per_off = 60;
  EEPROM.put(0, eeprom);
  // ---------------------------------------

   boolean ok2 = EEPROM.commit();
  //  terminal.println((ok2) ? "Commit OK" : "Commit failed");  
  // IDt_reconnectBlynk = timer.setInterval(10000, reconnectBlynk);
  // timer.setInterval(200, regul);
  // IDt_debug = timer.setInterval(10000, debug);
  // timer.setInterval(1000, send_json);
  // reconnectBlynk(); 
  ArduinoOTA.setHostname("ESP32"); // Задаем имя сетевого порта
  ArduinoOTA.begin(); 
  digitalWrite(PIN_LOW,HIGH);
  digitalWrite(PIN_HIGH,HIGH);
  digitalWrite(relay,LOW);
  digitalWrite(nasos_otop, LOW);
}


BLYNK_WRITE(V2) {
  // Уставка бойлера
 eeprom.temp_u = param.asFloat();
 char temp[10];
 sprintf(temp, "%4.1f", eeprom.temp_u);
 client.publish("/home/boy_on/ustavka/boy", temp, true);
 
}

BLYNK_WRITE(V4) {
  // Уставка опалення
  eeprom.temp_u_b = param.asFloat();
}

BLYNK_WRITE(V14) {
  // Гістерезіс бойлера
  eeprom.gis_boy = param.asFloat();
}

BLYNK_WRITE(V5) {
   eeprom.heat = param.asInt();
  if (param.asInt())
  {
    client.publish("/home/boy_on/state", "ON");
    client.publish("/home/boy_on", "ON");
  }
  else
  {
    client.publish("/home/boy_on/state", "OFF");
    client.publish("/home/boy_on", "OFF");
  }
  
}

BLYNK_WRITE(V15) {
  eeprom.heat_otop = param.asInt();
  if (param.asInt())
  {
    client.publish("/home/heat_on/state", "ON");
    client.publish("/home/heat_on/", "ON");
  }
  else
  {
    client.publish("/home/heat_on/state", "OFF");
    client.publish("/home/heat_on/", "OFF");
  }
}

BLYNK_WRITE(V18) {
  // eeprom.heat_otop = param.asInt();
deb = param.asInt();
}

BLYNK_WRITE(V19) {
  // eeprom.heat_otop = param.asInt();
run_mb = param.asInt();
}

BLYNK_WRITE(V17) {
  // Мінімальна температура відключення опалення
  eeprom.temp_off_otop = param.asFloat();
}


BLYNK_WRITE(V7) {
  // Цикл опалення
  eeprom.per_off = param.asInt();
  High.OffTime = eeprom.per_off;
  Low.OffTime = eeprom.per_off;
}

BLYNK_WRITE(V8) {
  // імпульс опалення
 eeprom.per_on = param.asInt();
 High.OnTime = eeprom.per_on;
 Low.OnTime = eeprom.per_on;
}

void loop()
{
  ArduinoOTA.handle(); // Всегда готовы к прошивке

#ifdef USE_LOCAL_SERVER
  if (Blynk.connected()){ 
   Blynk.run(); 
   
   EEPROM.put(0, eeprom);
   boolean ok2 = EEPROM.commit();
   isFirstConnection = false;
  }
#endif 


  if (isFirstConnection)
  {
  EEPROM.get(0, eeprom);
  // Blynk.syncAll(); 
  // if (Blynk.connected()){ 

  // }
  }

// timer.run();
 rssi =  map(WiFi.RSSI(), -115, -35, 0, 100);
//  
 unsigned long real_time = millis();
  if (real_time - old_time>1000)
    {
      old_time = real_time;
      T_koll = kollektor.Update_f();
      T_bat = bat.Update_f();
      T_boyler = boyler.Update_f();
      // temp_in();
    }
    if (real_time - old_time1>2000)
    {
      old_time1 = real_time;
      reconnect();
    }
    loopMQtt();
    if (real_time - old_time2>300)
    {
      old_time2 = real_time;
      regul();
    }

    if (real_time - old_time3 > 1000)
    {
      old_time3 = real_time;
      
    }
    
    // if (run_mb)    {
    
    if (real_time - timer4 > 5000)
    {
      timer4 = real_time;
      getValues();
      // loopMQtt();
    }
    // }

  regulator(T_koll, eeprom.temp_u_b, T_bat, eeprom.temp_off_otop);
// Deb_cont();

}