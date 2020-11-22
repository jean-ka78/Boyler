void kran_otop(uint8_t Time_run, uint8_t PIN)
{
  unsigned long newtime; 
  bool run = true;
  bool open, close;
   while (open)
   {
    if(!run){
     digitalWrite(PIN, LOW);
         
     Serial.println("работает кран");
     newtime = millis()+(Time_run*1000);
     }
    else if (millis() > newtime){
        digitalWrite(PIN, HIGH);// Выключить реле
        open = false;
        // close = true;
        run = false; 
        Serial.println("остановился кран");
        }
    }


}


void regulator(float temp_u_b, int PIN_HIGH, int PIN_LOW, float temp_bat, uint8_t Time_run)
{
if (temp_u_b>temp_bat)
{
    kran_otop(Time_run, PIN_HIGH);
}
else
{
    kran_otop(Time_run, PIN_LOW);
}

}