class temp
{
private:
    uint16_t time;
    DeviceAddress kolThermometer   = { 0x28, 0x64, 0xE1, 0x12, 0x5F, 0x14, 0x01, 0x84 };
    DeviceAddress boyThermometer   = { 0x28, 0xF5, 0x75, 0xC9, 0x2F, 0x14, 0x01, 0xD0 };
    DeviceAddress batThermometer   = { 0x28, 0xAA, 0xF0, 0x86, 0x13, 0x13, 0x02, 0x50 };

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
}