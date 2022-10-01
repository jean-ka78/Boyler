ModbusIP mb;  //ModbusIP object

const int REG = 528;               // Modbus Hreg Offset
int run_mb=0;
IPAddress remote(10, 1, 0, 5); 
void setup_mb ()
{
    mb.client(); 
}
uint16_t res = 0;

void loop_mb() {
  if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
    uint16_t trans = mb.readHreg(remote, REG, &res);  // Initiate Read Hreg from Modbus Server
    while(mb.isTransaction(trans)) {  // Check if transaction is active
      mb.task();
      delay(10);
    }
    terminal.println(res);          // At this point res is filled with responce value
  } else {
    terminal.println("not connect MB");
    mb.connect(remote);           // Try to connect if no connection
  }
  // delay(10);
}