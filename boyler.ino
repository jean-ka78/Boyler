
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
const int relay = 18;
const int PIN_LOW = 19;
const int PIN_HIGH = 23;


#include "heat_regul.h"
#include "obogrev.h"

// void adrr()
// {
//   byte i;
//   byte present = 0;
//   byte type_s;
//   byte data[12];
//   byte addr[8];
//   float celsius, fahrenheit;
  
//   if ( !oneWire.search(addr)) {
//     Pr.println("No more addresses.");
//     Pr.println();
//     oneWire.reset_search();
//     delay(250);
//     return;
//   }
  
//   Pr.print("ROM =");
//   for( i = 0; i < 8; i++) {
//     Pr.write(' ');
//     Pr.print(addr[i], HEX);
//   }

//   if (OneWire::crc8(addr, 7) != addr[7]) {
//       Pr.println("CRC is not valid!");
//       return;
//   }
//   Pr.println();
 
//   // the first ROM byte indicates which chip
//   switch (addr[0]) {
//     case 0x10:
//       Pr.println("  Chip = DS18S20");  // or old DS1820
//       type_s = 1;
//       break;
//     case 0x28:
//       Pr.println("  Chip = DS18B20");
//       type_s = 0;
//       break;
//     case 0x22:
//       Pr.println("  Chip = DS1822");
//       type_s = 0;
//       break;
//     default:
//       Pr.println("Device is not a DS18x20 family device.");
//       return;
//   } 

//   oneWire.reset();
//   oneWire.select(addr);
//   oneWire.write(0x44, 1);        // start conversion, with parasite power on at the end
  
//   delay(1000);     // maybe 750ms is enough, maybe not
//   // we might do a ds.depower() here, but the reset will take care of it.
  
//   present = oneWire.reset();
//   oneWire.select(addr);    
//   oneWire.write(0xBE);         // Read Scratchpad

//   Pr.print("  Data = ");
//   Pr.print(present, HEX);
//   Pr.print(" ");
//   for ( i = 0; i < 9; i++) {           // we need 9 bytes
//     data[i] = oneWire.read();
//     Pr.print(data[i], HEX);
//     Pr.print(" ");
//   }
//   Pr.print(" CRC=");
//   Pr.print(OneWire::crc8(data, 8), HEX);
//   Pr.println();

//   // Convert the data to actual temperature
//   // because the result is a 16 bit signed integer, it should
//   // be stored to an "int16_t" type, which is always 16 bits
//   // even when compiled on a 32 bit processor.
//   int16_t raw = (data[1] << 8) | data[0];
//   if (type_s) {
//     raw = raw << 3; // 9 bit resolution default
//     if (data[7] == 0x10) {
//       // "count remain" gives full 12 bit resolution
//       raw = (raw & 0xFFF0) + 12 - data[6];
//     }
//   } else {
//     byte cfg = (data[4] & 0x60);
//     // at lower res, the low bits are undefined, so let's zero them
//     if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
//     else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
//     else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
//     //// default is 12 bit resolution, 750 ms conversion time
//   }
//   celsius = (float)raw / 16.0;
//   fahrenheit = celsius * 1.8 + 32.0;
//   Pr.print("  Temperature = ");
//   Pr.print(celsius);
//   Pr.print(" Celsius, ");
//   Pr.print(fahrenheit);
//   Pr.println(" Fahrenheit");

// }

void setup()
{ 
   // Debug console
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
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

