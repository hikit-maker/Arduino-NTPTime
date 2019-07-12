# Arduino-NTPTime
NTP Time library for getting the UTC time from a remote server.

## Sample code:

```
/*
 *  This sketch syncs every 10 minutes the NTP time from "time.nist.gov"
 *  with the internal time
 *
 *  ps: Remember to change the SSID and PASSWORD
 *
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "NTPTime.hpp"

WiFiUDP udp;

const char* ssid = "your-ssid";
const char* password = "your-password";

void setup() {
    Serial.begin(115200);
    delay(100);
  
    Serial.print("\nConnecting to ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  
    Serial.println("\nWiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  
    NTPTime.begin(udp);
}

void loop() {
    // By default the time gets updated every 10 minutes
    // no need for more precision
    NTPTime.update();


    Serial.print(hour());
    Serial.print(":");
    Serial.print(minute());
    Serial.print(":");
    Serial.println(second());

    delay(1000);
}

```
