#include <WiFi.h>
#include <WiFiUdp.h>
#include <elapsedMillis.h>
#include <bits/stdc++.h>
#include <EEPROM.h>

const char * udpAddress = "255.255.255.255";
const int udpPort = 20001;
String msg = "";
String msg2 = "";
uint32_t start_time = 0;
uint32_t time_diff = 0;
uint32_t count = 0;
int len = 0;
int dataArr[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 0, 1);
WiFiUDP udp;
elapsedMillis timeout;

void setup() {
  Serial.begin(115200);
    if (!EEPROM.begin(1000)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
  else{
    Serial.println("EEPROM working fine");
  }
  int ip1 = EEPROM.readInt(0);
  int ip2 = EEPROM.readInt(4);
  int ip3 = EEPROM.readInt(8);
  int ip4 = EEPROM.readInt(12);
  IPAddress staticIP(ip1, ip2, ip3, ip4);

  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    Serial.println("Configuration failed.");
  }
    WiFi.begin("TP-Link_B61A", "msort@flexli");

  Serial.println("");
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
    
    if(count==1001){
      String temp="";
      for(int i =0; i<21;i++){
        Serial.println(i);
        Serial.println(dataArr[i]);
        temp=temp+"_"+String(i)+":"+String(dataArr[i]);
      }
      Serial.println(temp);
      EEPROM.writeString(16, temp);
        EEPROM.commit();
    }
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
        timeout = 100;
      }
    }
  }
  if (time_diff > 0) {
    Serial.println(msg);
    if (time_diff < 5) {
      dataArr[1]++;
    } else if (time_diff < 10) {
      dataArr[2]++;
    } else if (time_diff < 15) {
      dataArr[3]++;
    } else if (time_diff < 20) {
      dataArr[4]++;
    } else if (time_diff < 20) {
      dataArr[5]++;
    } else if (time_diff < 25) {
      dataArr[6]++;
    } else if (time_diff < 30) {
      dataArr[7]++;
    } else if (time_diff < 35) {
      dataArr[8]++;
    } else if (time_diff < 40) {
      dataArr[9]++;
    } else if (time_diff < 45) {
      dataArr[10]++;
    } else if (time_diff < 50) {
      dataArr[11]++;
    } else if (time_diff < 55) {
      dataArr[12]++;
    } else if (time_diff < 60) {
      dataArr[13]++;
    } else if (time_diff < 65) {
      dataArr[14]++;
    } else if (time_diff < 70) {
      dataArr[15]++;
    } else if (time_diff < 75) {
      dataArr[16]++;
    } else if (time_diff < 80) {
      dataArr[17]++;
    } else if (time_diff < 85) {
      dataArr[18]++;
    } else if (time_diff < 90) {
      dataArr[19]++;
    } else if (time_diff < 95) {

    } else if (time_diff < 100) {
      dataArr[20]++;
    }
  }
  else {
    Serial.println(msg2);
    dataArr[0]++;
  }
  time_diff = 0;

}