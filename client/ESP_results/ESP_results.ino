#include <EEPROM.h>
void setup() {
  Serial.begin(115200);
  if (!EEPROM.begin(1000)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  } else {
    Serial.println("EEPROM working fine");
  }

  Serial.println(EEPROM. readString(16));

}
void loop() {
}




// uint32_t Freq = 0;

// void setup() {
//   Serial.begin(115200);
//   Freq = getCpuFrequencyMhz();
//   Serial.print("CPU Freq = ");
//   Serial.print(Freq);
//   Serial.println(" MHz");
//   setCpuFrequencyMhz(80);
//   Freq = getCpuFrequencyMhz();
//   Serial.print("CPU Freq = ");
//   Serial.print(Freq);
//   Serial.println(" MHz");
//   Freq = getXtalFrequencyMhz();
//   Serial.print("XTAL Freq = ");
//   Serial.print(Freq);
//   Serial.println(" MHz");
//   Freq = getApbFrequency();
//   Serial.print("APB Freq = ");
//   Serial.print(Freq);
//   Serial.println(" Hz");
// }

// void loop() {
// }