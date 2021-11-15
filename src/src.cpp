#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// #include <ESP8266WiFi.h>
// #include <BlynkSimpleEsp8266.h>


#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include "NTC.h"

BlynkTimer timer;
// Первое ли это подключение к Серверу
  bool isFestConnection=true;
WidgetLED led1(V6), led2(V9), led3(V10), led4(V13), led5(V16);
WidgetTerminal terminal(V12);
// ID для таймеров Blynk
  int IDt_reconnectBlynk; // ID таймера для перезагрузки

/////////////////////////////////////////
//   База переменных для подключения   //
#include <OneWire.h>
#include <DallasTemperature.h>
#include "link.h"
// #include <ESP_EEPROM.h>
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
struct st_Enum
{

float  temp_u;     //Уставка бойлера
float  temp_u_b;   //Уставка баттарей
float temp_off_otop; //уставка отключения отопления
float gis_boy;  //gisterezis boyler
bool heat; //флаг включения бойлера
bool heat_otop; //Флаг включения отопления
uint32_t per_on; //Период включения 
uint32_t per_off; //Период виключения

} eeprom;




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
#include "json.h"

void temp_in()
{
  //  sensors.requestTemperatures();
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
  sensors.begin();
  EEPROM.begin(sizeof(st_Enum));
  
  EEPROM.put(0, eeprom);


  eeprom.temp_u = 50;
  eeprom.temp_u_b = 50;
  eeprom.temp_off_otop = 35;
  eeprom.gis_boy = -5;
  eeprom.heat = true;
  eeprom.heat_otop = true;
  eeprom.per_on = 10;
  eeprom.per_off = 60;
EEPROM.put(0, eeprom);

   boolean ok2 = EEPROM.commit();
   Serial.println((ok2) ? "Commit OK" : "Commit failed");
  // EEPROM.begin(512);
  sensors.setResolution(kolThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(boyThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(batThermometer, TEMPERATURE_PRECISION);
  timer.setInterval(1000, temp_in);
  // timer.setInterval(1000, temp_out);
  // timer.setInterval(500, temp_bat);
  IDt_reconnectBlynk = timer.setInterval(10000, reconnectBlynk);
  timer.setInterval(200, regul);
  timer.setInterval(1000, send_json);
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
digitalWrite(nasos_otop, LOW);
// T_koll = kollektor.Update_f();
// T_bat = bat.Update_f();
// T_boyler = boyler.Update_f();
// temp_u=EEPROM.read( 20);
// temp_u_b=EEPROM.read( 28);
// heat=EEPROM.read(44);
// heat_otop = EEPROM.read(52);
// gis_boy = EEPROM.read(36);
// temp_off_otop = EEPROM.read(60);

// per_off=30;
// per_on=1;

// timer.setInterval(900, kran_otop);
}


BLYNK_WRITE(V2) {
 eeprom.temp_u = param.asFloat();
  // EEPROM.write(20, temp_u);
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}

BLYNK_WRITE(V4) {
  eeprom.temp_u_b = param.asFloat();
  // EEPROM.write(28, temp_u_b);
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}

BLYNK_WRITE(V14) {
  eeprom.gis_boy = param.asFloat();
// EEPROM.write(64, gis_boy);
// EEPROM.put(60, gis_boy);
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
//  terminal.println("gis_blynk: "+String(gis_boy));
}


BLYNK_WRITE(V5) {
  eeprom.heat = param.asInt();
  // EEPROM.write(44, heat);
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}

BLYNK_WRITE(V15) {
  eeprom.heat_otop = param.asInt();
  // EEPROM.write(52, heat_otop);
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}

BLYNK_WRITE(V17) {
  eeprom.temp_off_otop = param.asFloat();
  // EEPROM.write(60, temp_off_otop);
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}


BLYNK_WRITE(V7) {
  eeprom.per_off = param.asInt();
  High.OffTime = eeprom.per_off;
  Low.OffTime = eeprom.per_off;
  // EEPROM.write(68, per_off);
  //digitalWrite(ledPin, ledState);
//   Serial.print(temp_u);
  //Serial.write((uint8_t*)&temp_u, sizeof(temp_u));
  //Serial.println("\t");
 // Send();
}
BLYNK_WRITE(V8) {
  eeprom.per_on = param.asInt();
 High.OnTime = eeprom.per_on;
 Low.OnTime = eeprom.per_on;
// EEPROM.write(76, per_on);
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
 if (tempC >= 1 && tempC <= 150.00)
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




void loop()
{
  ArduinoOTA.handle(); // Всегда готовы к прошивке
  if (Blynk.connected()){ 
    Blynk.run(); 
   Blynk.syncAll(); 
   EEPROM.put(0, eeprom);
   boolean ok2 = EEPROM.commit();
  //  Serial.println((ok2) ? "Commit OK" : "Commit failed");
  //  terminal.println((ok2) ? "Commit OK" : "Commit failed");
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
  // eeprom.per_off=60;
  // eeprom.per_on=10;
   
    
    // terminal.println(eeprom.temp_u);
    
  }
  timer.run();
 rssi =  map(WiFi.RSSI(), -115, -35, 0, 100);
 Blynk.virtualWrite(V11, rssi);
//  kran_otop();
 unsigned long real_time = millis();
  if (real_time - old_time>1000)
    {
      old_time = real_time;
      T_koll = kollektor.Update_f();
      // T_bat = bat.Update();
      // T_boyler = boyler.Update();
    }
    if (real_time - old_time1>2000)
    {
      old_time1 = real_time;
      // T_koll = kollektor.Update();
      T_bat = bat.Update_f();
      // T_boyler = boyler.Update();
    }
    if (real_time - old_time2>3000)
    {
      old_time2 = real_time;
      // T_koll = kollektor.Update();
      // T_bat = bat.Update();
      T_boyler = boyler.Update_f();


    }
       
    regulator(T_koll, eeprom.temp_u_b, T_bat, eeprom.temp_off_otop);

    
}


