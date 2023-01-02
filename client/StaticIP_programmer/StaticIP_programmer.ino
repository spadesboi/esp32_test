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
  EEPROM.writeInt(0, 192);
  EEPROM.writeInt(4, 168);
  EEPROM.writeInt(8, 0);
  EEPROM.writeInt(12, 127);
  EEPROM.writeInt(16, 0);
  EEPROM.commit();
  delay(1000);
  Serial.println(EEPROM.readInt(0));
  Serial.println(EEPROM.readInt(4));
  Serial.println(EEPROM.readInt(8));
  Serial.println(EEPROM.readInt(12));
  Serial.println(EEPROM.readInt(16));
}
void loop() {
}   
