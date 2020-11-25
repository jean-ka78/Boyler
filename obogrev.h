


bool regulator(float Temp_kol, float temp_u_b, float temp_b)
{
    // float Temp_kol = printTemperature(kolThermometer);
    float temp_on;
    bool kran;
    if (Temp_kol<temp_u_b)
    {
        temp_on = Temp_kol;
    }
    else
    {
        temp_on = temp_u_b;
    }
if (temp_on>=temp_b)
{
        kran = HIGH;
        led2.off();
        led3.on();
}
else
{
        kran = LOW;
        led2.on();
        led3.off();
}
 
 return kran;
}

void open(int Time, int Time1, int PIN)
{
// uint32_t tmr;

    if (millis() - tmr >= (flag ? Time*1000 : Time1*1000)) {
    tmr = millis();
    flag = !flag;
    digitalWrite(PIN, flag);
    // тут можно сделать digitalWrite(pin, flag);
    // для переключения реле
  }
// }
}


