#include <WiFi.h>
#include <WiFiUdp.h>
#include <elapsedMillis.h>
#include <bits/stdc++.h>
#include <EEPROM.h>
#include <HTTPClient.h>

const char *udpAddress = "192.168.0.200";
const int udpPort = 20001;
String msg = "";
String msg2 = "";
uint32_t start_time = 0;
uint32_t time_diff = 0;
uint32_t count = 0;
int len = 0;
int avgClock = 0;
int timeoutCount = 0;
int timeout = 100;
int dataArr[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int delayTime = 500;
int prevMilies = 0;
int datapoints = 100000;
int signalStrength = 0;
bool flag = false;
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 0, 1);
int disconnects = 0;
uint32_t disconnects_time = 0;
uint32_t disconnect_avg_time = 0;
uint32_t disconnect_avg_time_final = 0;
uint32_t reconnect_time = 0;
uint32_t max_disconnect_time = 0;
WiFiUDP udp;
elapsedMillis timer;
elapsedMillis internalclock;
HTTPClient http;


void setup() {
  setCpuFrequencyMhz(240);
  Serial.begin(115200);
  if (!EEPROM.begin(1000)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  } else {
    Serial.println("EEPROM working fine");
    Serial.println(EEPROM.readInt(16));
    Serial.println(EEPROM.readString(20));
  }

  int ip1 = EEPROM.readInt(0);
  int ip2 = EEPROM.readInt(4);
  int ip3 = EEPROM.readInt(8);
  int ip4 = EEPROM.readInt(12);
  IPAddress staticIP(ip1, ip2, ip3, ip4);
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    Serial.println("Configuration failed.");
  }
  // udp.setTimeout(100);
  WiFi.begin("TP-Link_B61A", "msort@flexli");

  Serial.println("");
  // Wait for connection
  for (int i = 0; i < 7; i++) {
    if (i == 6) {
      ESP.restart();
    }
    if (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    } else {
      i = 10;
      http.begin("http://192.168.0.200:8080/data");
    }
  }
  Serial.println(millis());
  Serial.print("Connected");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  udp.begin(udpPort);
  int randDelay = random(400, 1000);
  prevMilies = millis();
  while (millis() < prevMilies + randDelay) {
  }
}


void loop() {
  while (count < datapoints + 1) {
    start_time = millis();
    if (WiFi.status() == WL_CONNECTED) {
      // Serial.println(WiFi.RSSI());  //COMMENT
      signalStrength = WiFi.RSSI();
      if (signalStrength > -65) {
        // Serial.print("Signal: ");        //COMMENT
        // Serial.println(signalStrength);  //COMMENT
        // Serial.println(WiFi.BSSIDstr());  //COMMENT
        msg = String(count + datapoints) + "_" + String(start_time);
        msg2 = "";
        char *x = (char *)msg.c_str();
        internalclock = 0;
        UdpSend(x, "192.168.0.200", 20001);
        flag = false;
        UdpWaitAndRecive();
        avgClock = avgClock + internalclock;
        prevMilies = millis();
        while (millis() < prevMilies + delayTime) {
        }
        count++;
        if ((count % 201) == 0) {  //count == datapoints + 1
          if (disconnects) {
            disconnect_avg_time_final = disconnect_avg_time / disconnects;
          }
          String temp = "";
          for (int i = 0; i < 21; i++) {
            temp = temp + "_" + String(i) + ":" + String(dataArr[i]);
          }


          // Add for i=0, packet loss, divide by count, and store on EEPROM
          temp = temp + "__" + String(avgClock / count) + "_" + String((dataArr[0] / float(count)) * 100.0) + "%__" + String(millis()) + "__" + String(disconnects) + "_" + String(disconnect_avg_time_final) + "_" + String(max_disconnect_time) + "_" + String(signalStrength) + "_" + String(WiFi.BSSIDstr()) + "_" + String(EEPROM.readInt(12));
          EEPROM.writeString(20, temp);
          int programCount = EEPROM.readInt(16);
          programCount++;
          EEPROM.writeInt(16, programCount);
          EEPROM.commit();
          int httpCode = http.POST(temp);
          if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            // Serial.println(httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK) {
              String payload = http.getString();
              // Serial.println(payload);
            }
          } else {
            Serial.println(http.errorToString(httpCode).c_str());
          }
        }

        //updated code to reconnect on the basis of packet loss
        if (!flag) {
          // Serial.println("Packet lost, checking signal strength");  //COMMENT
          // Serial.print("Lost packets: ");   //comment
          // Serial.println(dataArr[0]);   //comment
          signalStrength = WiFi.RSSI();
          // Serial.println(signalStrength);  //comment
          if (signalStrength < -55) {
            // Serial.println("Signal strength LOW");  //COMMENT
            // Serial.print("Signal: ");               //COMMENT
            // Serial.println(signalStrength);         //COMMENT
            // Serial.println(WiFi.BSSIDstr());        //COMMENT
            WiFi.disconnect();
            while (WiFi.status() == WL_CONNECTED) {
            }
            WiFi.begin("TP-Link_B61A", "msort@flexli");
            int disconnect_loop_count = 0;
            for (int i = 0; i <= 7; i++) {  //blocking till connected within 1500ms or restart
              if (i == 6) {
                WiFi.disconnect();
                while (WiFi.status() == WL_CONNECTED) {
                }
                WiFi.begin("TP-Link_B61A", "msort@flexli");
                i = 0;
                disconnect_loop_count++;
                if (disconnect_loop_count == 3) {
                  // Serial.println("Restart");  //COMMENT
                  ESP.restart();
                }
              }

              reconnect_time = millis();
              if (WiFi.status() != WL_CONNECTED) {
                while (millis() - reconnect_time < 100) {
                }
                // Serial.print(".");
              } else {
                i = 10;
              }
            }
            disconnects_time = millis() - start_time;
            if (disconnects_time > max_disconnect_time) {
              max_disconnect_time = disconnects_time;
            }
            disconnect_avg_time = disconnect_avg_time + disconnects_time;
            // Serial.println(WiFi.BSSIDstr());
          }
        }
      } else {
        disconnects = disconnects + 1;
        // Serial.println("Signal strength lower than -65");  //COMMENT
        // Serial.println(WiFi.BSSIDstr());                   //COMMENT
        WiFi.disconnect();
        while (WiFi.status() == WL_CONNECTED) {
        }
        WiFi.begin("TP-Link_B61A", "msort@flexli");
        int disconnect_loop_count = 0;
        for (int i = 0; i <= 7; i++) {  //blocking till connected within 1500ms or restart
          if (i == 6) {
            WiFi.disconnect();
            while (WiFi.status() == WL_CONNECTED) {
            }
            WiFi.begin("TP-Link_B61A", "msort@flexli");
            i = 0;
            disconnect_loop_count++;
            if (disconnect_loop_count == 3) {
              // Serial.println("Restart");  //COMMENT
              ESP.restart();
            }
          }

          reconnect_time = millis();
          if (WiFi.status() != WL_CONNECTED) {
            while (millis() - reconnect_time < 100) {
            }
            // Serial.print(".");
          } else {
            i = 10;
          }
        }
        disconnects_time = millis() - start_time;
        if (disconnects_time > max_disconnect_time) {
          max_disconnect_time = disconnects_time;
        }
        disconnect_avg_time = disconnect_avg_time + disconnects_time;
        // Serial.println(WiFi.BSSIDstr());
      }

    } else {
      disconnects = disconnects + 1;
      // Serial.println("WiFi not connected, main loop");  //COMMENT
      // Serial.println(WiFi.BSSIDstr());                  //COMMENT
      WiFi.disconnect();
      while (WiFi.status() == WL_CONNECTED) {
      }
      WiFi.begin("TP-Link_B61A", "msort@flexli");
      int disconnect_loop_count = 0;
      for (int i = 0; i <= 7; i++) {  //blocking till connected within 1500ms or restart
        if (i == 6) {
          WiFi.disconnect();
          while (WiFi.status() == WL_CONNECTED) {
          }
          WiFi.begin("TP-Link_B61A", "msort@flexli");
          i = 0;
          disconnect_loop_count++;
          if (disconnect_loop_count == 3) {
            // Serial.println("Restart");  //COMMENT
            ESP.restart();
          }
        }

        reconnect_time = millis();
        if (WiFi.status() != WL_CONNECTED) {
          while (millis() - reconnect_time < 100) {
          }
          // Serial.print(".");
        } else {
          i = 10;
        }
      }
      disconnects_time = millis() - start_time;
      if (disconnects_time > max_disconnect_time) {
        max_disconnect_time = disconnects_time;
      }
      disconnect_avg_time = disconnect_avg_time + disconnects_time;
      // Serial.println(WiFi.BSSIDstr());
    }
  }
}


void UdpSend(const char *message, const char *ipaddress, int port) {
  udp.beginPacket(ipaddress, port);
  udp.print(message);
  udp.endPacket();
}


void UdpWaitAndRecive() {
  char incomingPacket[255];
  timer = 0;
  // bool flag = false;
  while (timer < timeout) {

    int packetSize = udp.parsePacket();

    if (packetSize) {
      len = udp.read(incomingPacket, 255);
      if (len > 0) {
        incomingPacket[len] = 0;
      }
      if (String(incomingPacket) == msg) {
        time_diff = millis() - start_time;
        msg = msg + "_" + String(time_diff);
        timer = timeout;
        flag = true;
      } else {
        msg2 = msg + "_" + String(millis() - start_time);
        // Serial.print("discarded packet : ");
        // Serial.println(String(incomingPacket));
        timer = 0;
      }
    }
  }
  if (!flag) {
    timeoutCount++;
    // Serial.println("timeout " + String(timeoutCount));
    // Serial.println(timeoutCount);
  }
  if (time_diff < 101) {

    // Serial.println(msg);
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
  } else {
    // Serial.println(msg2);
    dataArr[0]++;
  }
  time_diff = 101;
}
