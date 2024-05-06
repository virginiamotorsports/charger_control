#include <Arduino.h>
#include <Arduino_CAN.h>
#include <string.h>
#include <stdint.h>



uint16_t const max_voltage = 3984; // 3984 = 398.4V
uint16_t const max_current = 50; // 50 = 5.0A
uint8_t enable_data[] = {(max_voltage >> 8) & 0xFF, max_voltage & 0xFF, (max_current >> 8) & 0xFF, max_current & 0xFF,0x0};
uint8_t disable_data[] = {(max_voltage >> 8) & 0xFF, max_voltage & 0xFF, (max_current >> 8) & 0xFF, max_current & 0xFF,0x1};
CanMsg enable_msg = CanMsg(CanExtendedId(0x1806E5F4), 5, enable_data);
CanMsg disable_msg = CanMsg(CanExtendedId(0x1806E5F4), 5, disable_data);
uint16_t count = 0;

void setup() {
  
  Serial.begin(9600);
  while (!Serial) { }
  if (!CAN.begin(CanBitRate::BR_500k))
  {
    Serial.println("CAN.begin(...) failed.");
    for (;;) {}
  }
}

void loop() {

  if(count > 50){
    int ret = CAN.write(enable_msg);
    // Serial.println(enable_msg);
    if(!(ret == 0 || ret == 1)){
        Serial.print("CAN Error: ");
        Serial.println(ret);
        CAN.clearError();
    }
    count = 0;
  }

  if(CAN.available()){
    CanMsg msg = CAN.read();
    Serial.print("Received: ");
    float current_voltage = ((msg.data[0] << 8) | msg.data[1]) / 10.0;
    float current_amps = ((msg.data[2] << 8) | msg.data[3]) / 10.0;
    Serial.print("Voltage: ");
    Serial.print(current_voltage);
    Serial.print("V, Current: ");
    Serial.print(current_amps);
    Serial.println("A");
    uint8_t status = msg.data[4];
    Serial.print("Status: ");
    Serial.println(status & 0x1); //Hardware Fault
    Serial.println(status >> 0x1 & 0x1); // Charger OverTemp
    Serial.println(status >> 0x2 & 0x1); // Input voltage fault
    Serial.println(status >> 0x3 & 0x1); // Battery connection fault
    Serial.println(status >> 0x4 & 0x1); // communication fault

  }


  count++;
  delay(10);

}
