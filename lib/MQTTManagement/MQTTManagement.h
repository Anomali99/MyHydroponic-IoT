#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <functional>
#include <vector>
#include <string>
#include "NetworkManagement.h"

class MQTTManagement
{
private:
    NetworkManagement &_connection;
    PubSubClient _mqttClient;
    const char *_broker;
    int _port;
    unsigned long _lastConnectionCheck;
    String _clientId;
    std::vector<String> _topicsToSubscribe;
    void callback(char *topic, byte *payload, unsigned int length);

public:
    std::function<void(String, String)> messageReceivedCallback;
    std::function<void(bool)> statusCallback;
    MQTTManagement(
        NetworkManagement &connection,
        const char *broker,
        int port,
        std::vector<String> topics);
    void setup();
    void loop();
    bool publish(const char *topic, const char *payload);
    bool subscribe(const char *topic);
    void reconnect();
};
