
// #define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include "link.h"
#define ONE_WIRE_BUS 23
#define TEMPERATURE_PRECISION 12
WidgetLED led1(V6);
WidgetTerminal Pr(V8);
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
//  DeviceAddress kolThermometer, boyThermometer;

// Assign address manually. The addresses below will need to be changed
// to valid device addresses on your bus. Device address can be retrieved
// by using either oneWire.search(deviceAddress) or individually via
// sensors.getAddress(deviceAddress, index) 28 64 E1 12 5F 14 01 84; 28 F5 75 C9 2F 14 01 D0
DeviceAddress kolThermometer = { 0x28, 0x64, 0xE1, 0x12, 0x5F, 0x14, 0x01, 0x84 };
DeviceAddress boyThermometer   = { 0x28, 0xF5, 0x75, 0xC9, 0x2F, 0x14, 0x01, 0xD0 };
DeviceAddress batThermometer   = { 0x28, 0xAA, 0xF0, 0x86, 0x13, 0x13, 0x02, 0x50 };

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
// char auth[] = "R_8Reojduuwwsdw3xe-5FodBVxZzJU60";

// // Your WiFi credentials.
// // Set password to "" for open networks.
// char ssid[] = "UniNet";
// char pass[] = "owen2014";

BlynkTimer timer;
// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 1
float  temp_u;     //Уставка бойлера
float  temp_u_b;   //Уставка баттарей
bool heat;
const int relay = 21;
const int PIN_LOW = 19;
const int PIN_HIGH = 18;


#include "heat_regul.h"
#include "obogrev.h"


void setup()
{ 
   // Debug console
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  pinMode(PIN_LOW, OUTPUT);
  pinMode(PIN_HIGH, OUTPUT);
  sensors.begin();

  sensors.setResolution(kolThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(boyThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(batThermometer, TEMPERATURE_PRECISION);
  timer.setInterval(1000, temp_in);
  timer.setInterval(1000, temp_out);
  timer.setInterval(500, temp_bat);
  timer.setInterval(10000, reconnectBlynk);
  timer.setInterval(200, regul);
  // timer.setInterval(1000, adrr);
    reconnectBlynk(); 
//   Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
//   Blynk.begin(auth, ssid, pass, "greenhouse.net.ua", 8080);
 ArduinoOTA.setHostname("ESP32"); // Задаем имя сетевого порта
  //ArduinoOTA.setPassword((const char *)"0000"); // Задаем пароль доступа для удаленной прошивки
  ArduinoOTA.begin(); 

}


BLYNK_WRITE(V2) {
  temp_u = param.asFloat();
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}

BLYNK_WRITE(V4) {
  temp_u_b = param.asFloat();
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}


BLYNK_WRITE(V5) {
  heat = param.asInt();
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}

// function to print the temperature for a device
float printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Error: Could not read temperature data");

    // return;
  }

return tempC;

}

// function to print a device's resolution


void temp_in()
{
   sensors.requestTemperatures();
//   printAddress(insideThermometer);
  float t = printTemperature(kolThermometer);
float t_old;
float t_now;
if (t > 0 && t!=t_old)
{
t_old = t;
t_now = t_old;
   Blynk.virtualWrite(V0, t_now);
}
else
{
t_now = t_old;
// Blynk.virtualWrite(V0, t_now);
}
}

void temp_out()
{
sensors.requestTemperatures();
float t = printTemperature(boyThermometer);
float t_old;
float t_now;
if (t > 0 && t!=t_old)
{
t_old = t;
t_now = t_old;
   Blynk.virtualWrite(V1, t_now);
}
else
{
t_now = t_old;
//  Blynk.virtualWrite(V1, t_now);
}
}

void temp_bat()
{
sensors.requestTemperatures();
float t = printTemperature(batThermometer);
float t_old;
float t_now;
if (t > 0 && t!=t_old)
{
t_old = t;
t_now = t_old;
   Blynk.virtualWrite(V3, t_now);
}
else
{
t_now = t_old;
//  Blynk.virtualWrite(V1, t_now);
}
// return t_now;
}



void regul()
{
bool relle;
relle = logic(heat,printTemperature(boyThermometer),printTemperature(kolThermometer),temp_u);
digitalWrite(relay,relle);





}

void loop()
{
  ArduinoOTA.handle(); // Всегда готовы к прошивке
  if (Blynk.connected()){ Blynk.run(); Blynk.syncAll();}
  timer.run();

}

