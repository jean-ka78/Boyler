bool logic(bool heat_on, float Temp_boy, float Temp_kol, float temp_u)
{
    bool nasos = true;
    float temp_on;
    if (Temp_kol<temp_u)
    {
        temp_on = Temp_kol;
    }
    else
    {
        temp_on = temp_u;
    }
        
    if (heat_on)
    {
        if (Temp_boy<temp_on)
        {
           nasos = false;
           led1.on();
        }
        else
        {
            nasos = true;
            led1.off();
        }
        
    }
    else
    {
        nasos = true;
        led1.off();
    }
    return nasos;
}