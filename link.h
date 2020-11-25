#define USE_LOCAL_SERVER

#ifdef USE_LOCAL_SERVER
      //#define SERVER                  IPAddress(192, 168, 10, 9) // Свой IP пишите
      #define SERVER                  "greenhouse.net.ua" // Имя своего хоста 

#endif

/*
   Wifi Credentials
*/

// #define WIFI_SSID               "UniNet"  //Имя точки доступа WIFI VVK   AndroidAP
// #define WIFI_PASS               "owen2014" //пароль точки доступа WIFI vtb24admin 1234567890
#define WIFI_SSID               "aonline"  //Имя точки доступа WIFI VVK   AndroidAP
#define WIFI_PASS               "1qaz2wsx3edc" //пароль точки доступа WIFI vtb24admin 1234567890


/*
     Blynk Auth Code
*/
//#define AUTH                      "ccea663c6087406a89e48c7646adb86e"
#define AUTH                      "R_8Reojduuwwsdw3xe-5FodBVxZzJU60"


///////////////////////////////////////////////////////
//          Функции для подключения к Blynk          //

void ConnectBlynk()
{
  //*******************************************************
  // Запускаем WiFi
    if (WiFi.status() != WL_CONNECTED)// Если нет WiFi, то коннектимся
    {
      BLYNK_LOG(WIFI_SSID);
//      Serial.println();
//      Serial.println();
//      Serial.print("Connecting to ");
//      Serial.println(WIFI_SSID);
      
      WiFi.begin(WIFI_SSID, WIFI_PASS);

      int8_t count=0;
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        Serial.print(WiFi.status());
        count++;
        if (count >= 5){
          break;Serial.println("WiFi not connected");}
      }
    }
  /*  Дисконект - для справки 
    WiFi.disconnect(); // отключаемся от сети
    Serial.println("Disconnect WiFi.");
  */
  
  //*******************************************************
  // Запускаем Blynk
timer.disable(IDt_reconnectBlynk); // Выключаем таймер
  
  if (WiFi.status() == WL_CONNECTED)// Если нет WiFi, то не коннектимся
    {
          // CONNECT TO BLYNK
          #ifdef USE_LOCAL_SERVER // Если используются локальный сервер
            Blynk.config(AUTH, SERVER, 8080);
            Blynk.connect();
          #else // Иначе конектимся постаринке к блинку
            Blynk.config(AUTH);
            Blynk.connect();
          #endif
    }

  
  // До бесконечности будем оставаться в цикле while
  // пока не установим связь с сервером
  //  while (Blynk.connect() == false) {}
  timer.enable(IDt_reconnectBlynk); // Включаем таймер
  timer.restartTimer(IDt_reconnectBlynk); // Перезапускаем таймер
}//ConnectBlynk()


// Реконектимся если обрыв связи
void reconnectBlynk() {
  if (!Blynk.connected())
  {
    BLYNK_LOG("Disconnected now");
    ConnectBlynk();
    if (Blynk.connected()) 
    {
      BLYNK_LOG("Reconnected");      
    } 
    else 
    {
      BLYNK_LOG("Not reconnected");      
    }
  }
}//reconnectBlynk()
