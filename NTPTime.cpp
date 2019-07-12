#include "NTPTime.hpp"


void NTPTimeClass::begin(UDP &udp)
{
    this->begin(udp, 2390, 48);
}

void NTPTimeClass::begin(UDP &udp, int localPort, int packetSize)
{   
    if (_initialized)
    {
        return;
    }
    IPAddress timeServerIP;
    const char* ntpServerName = "time.nist.gov";
    WiFi.hostByName(ntpServerName, timeServerIP); 

    this->udp = &udp;
    
    this->udp->begin(localPort);

    this->timeServerIP = timeServerIP;

    this->updateTimeWindow = 600;
    this->packetWaitTimeWindow = 2;

    this->lastUpdate = -1;
    this->lastPacketRequest = -1;

    this->packetSize = packetSize;

    byte packetBuffer[ this->packetSize ];
    this->packetBuffer = packetBuffer;

    while ( this->lastUpdate == -1 )
    {
        this->update();
        delay(100);
    }
}

void NTPTimeClass::update()
{
    if ( this->shouldUpdateTime() )
    {
        this->sendNTPpacket(this->timeServerIP);
        this->lastPacketRequest = now();
    }
    
    if ( this->packetShouldHaveArrived() )
    {
        int packetSize = this->udp->parsePacket();

        if ( packetSize ) {
            unsigned long epoch = this->parseEpoch();
            setTime(epoch);
        }

        this->lastUpdate = now();
        this->lastPacketRequest = -1;
    }
}

bool NTPTimeClass::shouldUpdateTime()
{
    bool firstUpdate = this->lastUpdate == -1;
    bool packetNotRequested = this->lastPacketRequest == -1;
    bool timeWindowPassed = (now() - this->lastUpdate) > this->updateTimeWindow;
    
    return packetNotRequested && (firstUpdate || timeWindowPassed);
}

bool NTPTimeClass::packetShouldHaveArrived()
{
    bool packetRequested = this->lastPacketRequest > -1;
    bool timeWindowPassed = (now() - this->lastPacketRequest) > this->packetWaitTimeWindow;
    
    return packetRequested && timeWindowPassed;
}

void NTPTimeClass::sendNTPpacket(IPAddress& address)
{
    // set all bytes in the buffer to 0
    memset(this->packetBuffer, 0, this->packetSize);
    
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    this->packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    this->packetBuffer[1] = 0;     // Stratum, or type of clock
    this->packetBuffer[2] = 6;     // Polling Interval
    this->packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    this->packetBuffer[12]  = 49;
    this->packetBuffer[13]  = 0x4E;
    this->packetBuffer[14]  = 49;
    this->packetBuffer[15]  = 52;

    this->udp->beginPacket(address, 123);
    this->udp->write(this->packetBuffer, this->packetSize);
    this->udp->endPacket();
}

unsigned long NTPTimeClass::parseEpoch()
{
    this->udp->read(this->packetBuffer, this->packetSize);

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
    
    memset(this->packetBuffer, 0, this->packetSize);

    return epoch;
}

bool NTPTimeClass::expired(int newHour, int newMinute)
{
    return hour() < newHour || (hour() == newHour && minute() <= newMinute);
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TIMER)
    NTPTimeClass NTPTime;    
#endif
