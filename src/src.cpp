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



BlynkTimer timer;
// Первое ли это подключение к Серверу
  bool isFestConnection=true;
WidgetLED led1(V6), led2(V9), led3(V10), led4(V13), led5(V16);
WidgetTerminal terminal(V12);
// ID для таймеров Blynk
  int IDt_reconnectBlynk; // ID таймера для перезагрузки
#include "link.h"
#include "st_enum.h"

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
unsigned long old_time = 0;
unsigned long old_time1 = 0;
unsigned long old_time2 = 0;
#include "heat_regul.h"
#include "obogrev.h"

NTC kollektor(thermistorPin1);
NTC boyler(thermistorPin2);
NTC bat(thermistorPin3);
// #include "json.h"

void temp_in()
{
   Blynk.virtualWrite(V0, kollektor.Update_f());
   Blynk.virtualWrite(V1, boyler.Update_f());
   Blynk.virtualWrite(V3, bat.Update_f());
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
   // Debug console
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
   Serial.println((ok2) ? "Commit OK" : "Commit failed");  
  IDt_reconnectBlynk = timer.setInterval(10000, reconnectBlynk);
  timer.setInterval(200, regul);
  timer.setInterval(1200, temp_in);
  // timer.setInterval(1000, send_json);
  reconnectBlynk(); 
  ArduinoOTA.setHostname("ESP32"); // Задаем имя сетевого порта
  ArduinoOTA.begin(); 
digitalWrite(PIN_LOW,HIGH);
digitalWrite(PIN_HIGH,HIGH);
digitalWrite(relay,LOW);
digitalWrite(nasos_otop, LOW);
}i


BLYNK_WRITE(V2) {
 eeprom.temp_u = param.asFloat();
}

BLYNK_WRITE(V4) {
  eeprom.temp_u_b = param.asFloat();
}

BLYNK_WRITE(V14) {
  eeprom.gis_boy = param.asFloat();
}

BLYNK_WRITE(V5) {
  eeprom.heat = param.asInt();
}

BLYNK_WRITE(V15) {
  eeprom.heat_otop = param.asInt();
}

BLYNK_WRITE(V17) {
  eeprom.temp_off_otop = param.asFloat();
}
BLYNK_WRITE(V7) {
  eeprom.per_off = param.asInt();
  High.OffTime = eeprom.per_off;
  Low.OffTime = eeprom.per_off;
 }

BLYNK_WRITE(V8) {
  eeprom.per_on = param.asInt();
 High.OnTime = eeprom.per_on;
 Low.OnTime = eeprom.per_on;
}

void loop()
{
  ArduinoOTA.handle(); // Всегда готовы к прошивке
  if (Blynk.connected()){ 
   Blynk.run(); 
   Blynk.syncAll(); 
   EEPROM.put(0, eeprom);
   boolean ok2 = EEPROM.commit();
   isFestConnection = false;
  }

  if (isFestConnection)
  {
    /* code */
  EEPROM.get(0, eeprom);
  // eeprom.temp_u=50;
  Serial.print("temp_u: "+String(eeprom.temp_u));
  // eeprom.temp_u_b=50;
  Serial.print(" temp__b: "+String(eeprom.temp_u_b));
  // eeprom.heat = true;
  Serial.print("heat: "+String(eeprom.heat));
  // eeprom.heat_otop = true;
  Serial.print("heat_otop: "+String(eeprom.heat_otop));
  // eeprom.gis_boy = -5;
  Serial.print("gisterezis: "+String(eeprom.gis_boy));
  // eeprom.temp_off_otop = 35;
  Serial.print("temp_off: "+String(eeprom.temp_off_otop));
  }

timer.run();
 rssi =  map(WiFi.RSSI(), -115, -35, 0, 100);
 Blynk.virtualWrite(V11, rssi);
 unsigned long real_time = millis();
  if (real_time - old_time>1000)
    {
      old_time = real_time;
      T_koll = kollektor.Update_f();
    }
    if (real_time - old_time1>2000)
    {
      old_time1 = real_time;
      T_bat = bat.Update_f();
    }
    if (real_time - old_time2>3000)
    {
      old_time2 = real_time;
      T_boyler = boyler.Update_f();
    }
  regulator(T_koll, eeprom.temp_u_b, T_bat, eeprom.temp_off_otop);
}


