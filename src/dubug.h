 int IDt_debug; //ID таймера отладки
 int deb;
void debug()
{
  terminal.println("temp_u: "+String(eeprom.temp_u));
  // eeprom.temp_u_b=50;
  terminal.println("temp__b: "+String(eeprom.temp_u_b));
  // eeprom.heat = true;
  terminal.println("heat: "+String(eeprom.heat));
  // eeprom.heat_otop = true;
  terminal.println("heat_otop: "+String(eeprom.heat_otop));
  // eeprom.gis_boy = -5;
  terminal.println("gisterezis: "+String(eeprom.gis_boy));
  // eeprom.temp_off_otop = 35;
  terminal.println("temp_off: "+String(eeprom.temp_off_otop));

}

void Deb_cont()
{
    if (deb)
    {
     timer.enable(IDt_debug);
    //  timer.restartTimer(IDt_debug);   
    }
    else
    {
            timer.disable(IDt_debug);
            terminal.clear();

    }
}