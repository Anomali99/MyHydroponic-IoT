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
    const char *_username;
    const char *_password;
    int _port;
    String _clientId;
    unsigned long _lastConnectionCheck;
    const char *_willTopic = "device/status";
    const char *_willPayload = "0";
    std::vector<String> _topicsToSubscribe = {"environment/refresh",
                                              "pump/automation",
                                              "pump/manually"};
    unsigned long _activityStartTime = 0;
    const unsigned long ACTIVITY_DURATION = 100;
    void callback(char *topic, byte *payload, unsigned int length);

public:
    std::function<void(String, String)> messageReceivedCallback;
    std::function<void(bool)> statusCallback;
    MQTTManagement(NetworkManagement &connection);
    void setup();
    void loop();
    bool publish(const char *topic, const char *payload, bool retain = false);
    bool subscribe(const char *topic);
    void reconnect();
};
