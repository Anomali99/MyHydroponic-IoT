#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "Config.h"

enum ConnectionState
{
    IDLE,
    DISCONNECTED,
    CONNECTING,
    CONNECTED
};

class NetworkManagement
{
private:
    const char *NTP_SERVER;
    const long GMTOFFSET_SEC;
    const int DAYLIGHTOFFSET_SEC = 0;
    WiFiClient _wifiClient;

public:
    std::function<void(bool)> statusCallback;
    ConnectionState currentState;
    NetworkManagement();
    void setup();
    void loop();
    void reconnect();
    bool isConnected();
    WiFiClient &getClient();
    String getCurrentTime(const char *format = "%Y-%m-%d %H:%M:%S");
};