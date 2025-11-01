#pragma once
#include <Arduino.h>
#include <WiFi.h>

class NetworkManagement
{
private:
    const char *NTP_SERVER = "pool.ntp.org";
    const long GMTOFFSET_SEC = 7 * 3600;
    const int DAYLIGHTOFFSET_SEC = 0;
    WiFiClient _wifiClient;
    unsigned long _lastConnectionCheck;
    void checkInternetConnection();

public:
    std::function<void(bool)> statusCallback;
    std::function<void(bool)> statusInternetCallback;
    NetworkManagement(
        const char *NTP_SERVER = "pool.ntp.org",
        const long GMTOFFSET = 7,
        const int DAYLIGHTOFFSET_SEC = 0);
    void setup(const char *ssid,
               const char *pass);
    void loop();
    void reconnect();
    bool isConnected();
    WiFiClient &getClient();
    String getCurrentTime(const char *format = "%Y-%m-%d %H:%M:%S");
};