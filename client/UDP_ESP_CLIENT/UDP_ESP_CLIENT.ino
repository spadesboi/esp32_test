
//comparison and start and stop time and sending to data acc

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <elapsedMillis.h>
#include <bits/stdc++.h>

const char * udpAddress = "255.255.255.255";
const int udpPort = 20001;
String msg = "";
String msg2 = "";
uint32_t start_time = 0;
uint32_t time_diff = 0;
uint32_t count = 0;
int len = 0;

WiFiUDP udp;
elapsedMillis timeout;


void setup() {
  Serial.begin(115200);

  //  WiFi.begin("MadhusudhanRamesh", "spadesboi");
  //  WiFi.begin("TP-Link_35E3", "msort@flexli");
  WiFi.begin("TP-Link_B61A", "msort@flexli");
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  udp.begin(udpPort);
}


void loop()
{
  while (count < 1001) {
    start_time = millis();
    msg = String(count) + "_" + String(start_time);
    msg2 = "";
    char *x = (char *)msg.c_str();
    UdpSend(x, "255.255.255.255", 20001);
    UdpWaitAndRecive();
    count++;
  }
}


void UdpSend(const char* message, const char * ipaddress, int port) {
  udp.beginPacket(ipaddress, port);
  udp.print(message);
  udp.endPacket();
}


void UdpWaitAndRecive() {
  char incomingPacket[255];
  bool readPacket = false;
  timeout = 0;
  while (timeout < 50) {
    int packetSize = udp.parsePacket();
    if (packetSize)
    {
      len = udp.read(incomingPacket, 255);
      if (len > 0)
      {
        incomingPacket[len] = 0;
      }
      if (String(incomingPacket) == msg) {
        time_diff = millis() - start_time;
        msg = msg + "_" + String(time_diff);
        timeout = 100;
      }
      else {
        msg2 = msg + "_" + String(time_diff);
      }
    }
  }
  if (time_diff > 0) {
    Serial.println(msg);
  }
  else {
    Serial.println(msg2);
  }
  time_diff = 0;

}
