#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include "NTC.h"
BlynkTimer timer;
// Первое ли это подключение к Серверу
  bool isFestConnection=true;
WidgetLED led1(V6), led2(V9), led3(V10), led4(V13);
WidgetTerminal terminal(V12);
// ID для таймеров Blynk
  int IDt_reconnectBlynk; // ID таймера для перезагрузки

/////////////////////////////////////////
//   База переменных для подключения   //
#include <OneWire.h>
#include <DallasTemperature.h>
#include "link.h"
#define ONE_WIRE_BUS 15
#define TEMPERATURE_PRECISION 12


OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
//  DeviceAddress kolThermometer, boyThermometer, batThermometer;

// Assign address manually. The addresses below will need to be changed
// to valid device addresses on your bus. Device address can be retrieved
// by using either oneWire.search(deviceAddress) or individually via
// sensors.getAddress(deviceAddress, index);// 28 64 E1 12 5F 14 01 84; 28 F5 75 C9 2F 14 01 D0
DeviceAddress kolThermometer   = { 0x28, 0x64, 0xE1, 0x12, 0x5F, 0x14, 0x01, 0x84 };
DeviceAddress boyThermometer   = { 0x28, 0xF5, 0x75, 0xC9, 0x2F, 0x14, 0x01, 0xD0 };
DeviceAddress batThermometer   = { 0x28, 0xAA, 0xF0, 0x86, 0x13, 0x13, 0x02, 0x50 };

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
// char auth[] = "R_8Reojduuwwsdw3xe-5FodBVxZzJU60";

// // Your WiFi credentials.
// // Set password to "" for open networks.
// char ssid[] = "UniNet";
// char pass[] = "owen2014";


// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 1
float  temp_u;     //Уставка бойлера
float  temp_u_b;   //Уставка баттарей
uint32_t gis_boy;  //gisterezis boyler
bool heat;
int thermistorPin1 = 33;// Вход АЦП, выход делителя напряжения
int thermistorPin2 = 32;
int thermistorPin3 = 35;
const int relay = 21;
int PIN_LOW = 22;
int PIN_HIGH = 23;
uint32_t tmr;
bool flag = HIGH;
uint32_t per_on;
uint32_t per_off;
float T_boyler, T_koll, T_bat;
long rssi;
unsigned long old_time = 0;
unsigned long old_time1 = 0;
unsigned long old_time2 = 0;
#include "heat_regul.h"
#include "obogrev.h"

NTC kollektor(thermistorPin1, 50);
NTC boyler(thermistorPin2, 50);
NTC bat(thermistorPin3, 50);

void setup()
{ 
   // Debug console
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  // pinMode(PIN_LOW, OUTPUT);
  // pinMode(PIN_HIGH, OUTPUT);
  sensors.begin();

  sensors.setResolution(kolThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(boyThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(batThermometer, TEMPERATURE_PRECISION);
  timer.setInterval(1000, temp_in);
  // timer.setInterval(1000, temp_out);
  // timer.setInterval(500, temp_bat);
  IDt_reconnectBlynk = timer.setInterval(10000, reconnectBlynk);
  timer.setInterval(200, regul);
  // timer.setInterval(5000, regulator(kollektor.Update(),temp_u_b, bat.Update()));
    reconnectBlynk(); 
//   Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
//   Blynk.begin(auth, ssid, pass, "greenhouse.net.ua", 8080);
 ArduinoOTA.setHostname("ESP32"); // Задаем имя сетевого порта
  //ArduinoOTA.setPassword((const char *)"0000"); // Задаем пароль доступа для удаленной прошивки
  ArduinoOTA.begin(); 
 digitalWrite(PIN_LOW,HIGH);
 digitalWrite(PIN_HIGH,HIGH);
 digitalWrite(relay,LOW);
temp_u=EEPROM.read( 20);
temp_u_b=EEPROM.read( 28);
heat=EEPROM.read( 36);
gis_boy = EEPROM.read(60);
per_off=30;
per_on=1;
terminal.println("gisterezis: "+String(gis_boy));
// timer.setInterval(900, kran_otop);
}


BLYNK_WRITE(V2) {
  temp_u = param.asFloat();
  EEPROM.write(20, temp_u);
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}

BLYNK_WRITE(V4) {
  temp_u_b = param.asFloat();
  EEPROM.write(28, temp_u_b);
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}

BLYNK_WRITE(V14) {
  gis_boy = param.asFloat();

EEPROM.put(60, gis_boy);
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}


BLYNK_WRITE(V5) {
  heat = param.asInt();
  EEPROM.write(36, heat);
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}

BLYNK_WRITE(V7) {
  per_off = param.asInt();
  High.OffTime = per_off;
  Low.OffTime = per_off;
  EEPROM.write(44, per_off);
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}
BLYNK_WRITE(V8) {
  per_on = param.asInt();
 High.OnTime = per_on;
 Low.OnTime = per_on;
EEPROM.write(52, per_on);
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}

// function to print the temperature for a device
float printTemperature(DeviceAddress deviceAddress)
{
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(deviceAddress);
  float result;
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
     terminal.println("Error: Could not read temperature data");

    // return;
  }
 if (tempC >= 1 & tempC <= 150.00)
  {
   result = tempC;
  }
  else
  {
    result = 0;
  }
  
return result;

}

// function to print a device's resolution


void temp_in()
{
  //  sensors.requestTemperatures();
   Blynk.virtualWrite(V0, T_koll);
   Blynk.virtualWrite(V1, T_boyler);
   Blynk.virtualWrite(V3, T_bat);
}

void regul()
{
bool relle;
relle = logic(heat,T_boyler,T_koll,temp_u, gis_boy);
if (!relle)
{
  led1.off();
} else 
{
  led1.on();
}
digitalWrite(relay,relle);
}

void loop()
{
  ArduinoOTA.handle(); // Всегда готовы к прошивке
  if (Blynk.connected()){ Blynk.run(); Blynk.syncAll();}
  timer.run();
 rssi =  map(WiFi.RSSI(), -115, -35, 0, 100);
 Blynk.virtualWrite(V11, rssi);
//  kran_otop();
 unsigned long real_time = millis();
  if (real_time - old_time>5000)
    {
      old_time = real_time;
      T_koll = kollektor.Update();
      // T_bat = bat.Update();
      // T_boyler = boyler.Update();
    }
    if (real_time - old_time1>10000)
    {
      old_time1 = real_time;
      // T_koll = kollektor.Update();
      T_bat = bat.Update();
      // T_boyler = boyler.Update();
    }
    if (real_time - old_time2>7000)
    {
      old_time2 = real_time;
      // T_koll = kollektor.Update();
      // T_bat = bat.Update();
      T_boyler = boyler.Update();
    }
    
    
    regulator(T_koll,temp_u_b, T_bat);
}


