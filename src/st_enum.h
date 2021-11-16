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