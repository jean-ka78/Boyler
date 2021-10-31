class Flasher
{
   public:
  // Переменные - члены класса
  // Инициализируются при запуске
  int Kran_Pin; // номер пина со приводом
  unsigned long OnTime; // время включения в миллисекундах
  unsigned long OffTime; // время, когда привод выключен
  bool run_on = false;
  bool run_off = false;
  // Текущее состояние
  bool Kran_State = HIGH; // состояние ВКЛ/ВЫКЛ
  unsigned long previousMillis; // последний момент смены состояния
 
  // Конструктор 
  
  Flasher(int pin)
  {
   Kran_Pin = pin;
  //  run = Run;
   pinMode(Kran_Pin, OUTPUT);
 
//    OnTime = on*1000;
//    OffTime = off*1000;
 
   Kran_State = HIGH;
   previousMillis = 0;
  }
 
  void Update()
  {
   // выясняем не настал ли момент сменить состояние привода
//    OnTime = on*1000;
//    OffTime = off*1000;
   unsigned long currentMillis = millis(); // текущее время в миллисекундах
//  while (run)
//  {
    if((Kran_State == LOW) && (currentMillis - previousMillis >= OnTime*1000))
   {
     terminal.println("Update+OnTime:"+String(OnTime));
     Kran_State = HIGH; // выключаем
     previousMillis = currentMillis; // запоминаем момент времени
     digitalWrite(Kran_Pin, Kran_State); // реализуем новое состояние
     run_on = false;
     led4.off();
    //  led3.on();


   }
   else if ((Kran_State == HIGH) && (currentMillis - previousMillis >= OffTime*1000))
   {
     terminal.println("Update+OffTime:"+String(OffTime));
     Kran_State = LOW; // включаем
     previousMillis = currentMillis ; // запоминаем момент времени
     digitalWrite(Kran_Pin, Kran_State); // реализуем новое состояние
      led4.on();
      // led3.off();
      run_on = true;
   }
  // }
  }
};

Flasher High(PIN_HIGH);
Flasher Low(PIN_LOW);



void regulator(float Temp_kol, float temp_u_b, float temp_b, float temp_off)
{
    // float Temp_kol = printTemperature(kolThermometer);
    float temp_on;
    temp_on = temp_u_b;
    // bool kran;
if (heat_otop)
{
    if (Temp_kol>=temp_off)
{


    digitalWrite(nasos_otop, heat_otop);
   led5.on();
    // if (Temp_kol<temp_u_b)
    // {
    //     temp_on = Temp_kol;
    // }
    // else
    // {
    //     temp_on = temp_u_b;
    // }

if (((temp_on>=temp_b || temp_on==0) && (Low.Kran_State == HIGH) )|| (High.Kran_State == LOW))
{
        
        High.Update();
        led2.off();
        led3.on();
}
else if ((High.Kran_State == HIGH) || (Low.Kran_State == LOW))
{
        
        Low.Update();
        
        led2.on();
        led3.off();
}
}
else
{
heat_otop = LOW;
digitalWrite(nasos_otop, heat_otop);
led5.off();
}
}
else
{
    digitalWrite(nasos_otop, heat_otop);
    led5.off();
}

//  return kran;
}