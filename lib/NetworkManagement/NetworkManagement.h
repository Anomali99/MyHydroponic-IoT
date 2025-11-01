#pragma once
#include <Arduino.h>
#include <WiFi.h>

class NetworkManagement
{

private:
    WiFiClient _wifiClient;

public:
    std::function<void(bool)> statusCallback;
    NetworkManagement();
    void setup(const char *ssid,
               const char *pass);
    void loop();
    void reconnect();
    bool isConnected();
    WiFiClient &getClient();
};