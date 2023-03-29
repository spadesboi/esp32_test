/*
//              This code is only for testing purposes, it has scope for cleaning up and reducing code
//              but that is not the main goal of this program. This exact code has been implemented and
//              tested vigorously for different networking cases, documentation can be found here https://docs.google.com/document/d/1sb1OvSUDNEbbsOGarEihRbbwJHN0u8XkJnFvu_x6hfc/edit?usp=sharing
//              For more detailed results and data refer to the following link, https://docs.google.com/spreadsheets/d/1Z7HOo94GeRYyIfOGGlbkMw6JOHjLclBOYo3_hFHk3NU/edit?usp=sharing
//              Do NOT attempt to modify this code and then validate your results with the documented results.
*/

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
int timeout = 100;                                                                        // 100ms timeout for UdpWaitAndRecive function
int dataArr[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };        // To store the number of packets for different latency intervals
int delayTime = 500;                                                                      // Time after which a new packet is sent
int prevMilies = 0;
int datapoints = 100000;                                                                  // Total number of packets sent after which program stops
int signalStrength = 0;
bool flag = false;                                                                        // Flag to check if packet is lost
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress dns(192, 168, 0, 1);
int disconnects = 0;
uint32_t disconnects_time = 0;
uint32_t disconnect_avg_time = 0;                                                         // Stores the summation of all disconnect times
uint32_t disconnect_avg_time_final = 0;                                                   // Averages "disconnect_avg_time" over loop counter "count"
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
  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());
  udp.begin(udpPort);
  int randDelay = random(400, 1000);
  prevMilies = millis();
  while (millis() < prevMilies + randDelay) {                                             // Giving a random delay so that all ESP do not start at the same time
  }
}


void loop() {
  while (count < datapoints + 1) {
    start_time = millis();
    if (WiFi.status() == WL_CONNECTED) {
      // Serial.println(WiFi.RSSI());  //COMMENT
      signalStrength = WiFi.RSSI();
      if (signalStrength > -65) {                                                         // Setting a signal strength threshold of -65dB
        // Serial.print("Signal: ");        //COMMENT
        // Serial.println(signalStrength);  //COMMENT
        // Serial.println(WiFi.BSSIDstr());  //COMMENT
        msg = String(count + datapoints) + "_" + String(start_time);                      // Payload of UDP packet, includes the count (in the form of count + data points) and the time at which the loop started
        msg2 = "";
        char *x = (char *)msg.c_str();                                                    // Changing data type to a form that UDP library can take as an argument
        internalclock = 0;
        UdpSend(x, "192.168.0.200", 20001);
        flag = false;
        UdpWaitAndRecive();
        avgClock = avgClock + internalclock;                                              // Variable to store summation of latency of UDP packets between ESP and server
        prevMilies = millis();
        while (millis() < prevMilies + delayTime) {                                       // Adding a delay of "delayTime" every loop
        }
        count++;
        if ((count % 201) == 0) {                                                         // Every 200 packets sent, an HTTP message is sent to the server containing the statistics of the data up until this point
          if (disconnects) {
            disconnect_avg_time_final = disconnect_avg_time / disconnects;
          }
          String temp = "";
          for (int i = 0; i < 21; i++) {                                                  // Adding all latency data to a string from 0 to 100ms
            temp = temp + "_" + String(i) + ":" + String(dataArr[i]);
          }


          // Add average latency, packet loss %, time elapsed since start of ESP, number of disconnects, avg disconnect time, maximum disconnect time, signal strength and MAC address connected to in this loop, and the last 3 number of the ESP's IP
          temp = temp + "__" + String(avgClock / count) + "_" + String((dataArr[0] / float(count)) * 100.0) + "%__" + String(millis()) + "__" + String(disconnects) + "_" + String(disconnect_avg_time_final) + "_" + String(max_disconnect_time) + "_" + String(signalStrength) + "_" + String(WiFi.BSSIDstr()) + "_" + String(EEPROM.readInt(12));
          EEPROM.writeString(20, temp);
          int programCount = EEPROM.readInt(16);
          programCount++;
          EEPROM.writeInt(16, programCount);
          EEPROM.commit();
          int httpCode = http.POST(temp);                                                 // Sending HTTP POST request
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

        if (!flag) {                                                                      // In case of a packet loss
          // Serial.println("Packet lost, checking signal strength");  //COMMENT
          // Serial.print("Lost packets: ");   //comment
          // Serial.println(dataArr[0]);   //comment
          signalStrength = WiFi.RSSI();
          // Serial.println(signalStrength);  //comment
          if (signalStrength < -55) {                                                     // Signal threshold here is -55
            disconnects = disconnects + 1;
            // Serial.println("Signal strength LOW");  //COMMENT
            // Serial.print("Signal: ");               //COMMENT
            // Serial.println(signalStrength);         //COMMENT
            // Serial.println(WiFi.BSSIDstr());        //COMMENT
            WiFi.disconnect();
            while (WiFi.status() == WL_CONNECTED) {                                       // Wait until disconnected
            }
            WiFi.begin("TP-Link_B61A", "msort@flexli");
            int disconnect_loop_count = 0;
            for (int i = 0; i <= 7; i++) {                                                // Blocking till connected within 1800ms or restart
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
      } else {                                                                            // Same logic as the other disconnect loops
        disconnects = disconnects + 1;
        // Serial.println("Signal strength lower than -65");  //COMMENT
        // Serial.println(WiFi.BSSIDstr());                   //COMMENT
        WiFi.disconnect();
        while (WiFi.status() == WL_CONNECTED) {
        }
        WiFi.begin("TP-Link_B61A", "msort@flexli");
        int disconnect_loop_count = 0;
        for (int i = 0; i <= 7; i++) {                                                    // Blocking till connected within 1800ms or restart
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

    } else {                                                                              // If WiFi not connected, initate a disconnect loop
      disconnects = disconnects + 1;
      // Serial.println("WiFi not connected, main loop");  //COMMENT
      // Serial.println(WiFi.BSSIDstr());                  //COMMENT
      WiFi.disconnect();
      while (WiFi.status() == WL_CONNECTED) {
      }
      WiFi.begin("TP-Link_B61A", "msort@flexli");
      int disconnect_loop_count = 0;
      for (int i = 0; i <= 7; i++) {                                                      // Blocking till connected within 1500ms or restart
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


void UdpSend(const char *message, const char *ipaddress, int port) {                      // Function to send UDP packet
  udp.beginPacket(ipaddress, port);
  udp.print(message);
  udp.endPacket();                                                                        // Prepares the packet and sends it to the ipaddress and port specified
}


void UdpWaitAndRecive() {                                                                 // Waits for any packet, restarts timer if wrong packet is received, if no packet is received with timer, packet is considered to be lost
  char incomingPacket[255];
  timer = 0;                                                                              // elaspedMillis object
  // bool flag = false;
  while (timer < timeout) {                                                               // timeout set to 100ms

    int packetSize = udp.parsePacket();

    if (packetSize) {
      len = udp.read(incomingPacket, 255);
      if (len > 0) {
        incomingPacket[len] = 0;                                                          // Setting only the position of array after length of packet as 0, or else grabage characters in that position will give undefined errors
      }
      if (String(incomingPacket) == msg) {                                                // Checking to see if incomingPacket is the packet we sent in this loop
        time_diff = millis() - start_time;                                                // Calculating latency for ESP to send and receieve packet
        msg = msg + "_" + String(time_diff);
        timer = timeout;
        flag = true;
      } else {                                                                            // timer reset to 0 if wrong packet is received
        msg2 = msg + "_" + String(millis() - start_time);
        // Serial.print("discarded packet : ");
        // Serial.println(String(incomingPacket));
        timer = 0;
      }
    }
  }
  if (!flag) {                                                                            // If packet loss has ocurred, increment timeoutCount variable
    timeoutCount++;
    // Serial.println("timeout " + String(timeoutCount));
    // Serial.println(timeoutCount);
  }
  if (time_diff < 101) {                                                                  // Conditions to increment dataArr[x] according to the latency obtained

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
