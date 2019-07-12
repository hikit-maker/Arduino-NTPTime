#ifndef __TIMER_H_

#define __TIMER_H_

#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

class NTPTimeClass {
    public:
        void begin(UDP &udp);
        void begin(UDP &udp, int localPort, int packetSize);
        bool expired(int newHour, int newMinute);

        int update();

    private:
        int updateTimeWindow;
        int packetSize;
        int packetWaitTimeWindow;
        IPAddress timeServerIP;
        byte *packetBuffer;
        UDP *udp;
        time_t lastUpdate;
        time_t lastPacketRequest;
        bool _initialized;
        void sendNTPpacket(IPAddress& address);
        bool shouldUpdateTime();
        bool packetShouldHaveArrived();
        unsigned long parseEpoch();
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TIMER)
    extern NTPTimeClass NTPTime;
#endif

#endif  // __TIMER_H_
