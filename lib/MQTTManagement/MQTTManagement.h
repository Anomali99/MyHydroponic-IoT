#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <functional>
#include <vector>
#include <string>
#include "NetworkManagement.h"
#include "Config.h"

class MQTTManagement
{
private:
    NetworkManagement &_connection;
    PubSubClient _mqttClient;
    const char *_broker;
    const char *_username;
    const char *_password;
    const char *_willTopic = "device/status";
    const char *_willPayload = "0";
    int _port;
    String _clientId;
    unsigned long _lastConnectionCheck;
    std::vector<String> _topicsToSubscribe;
    void callback(char *topic, byte *payload, unsigned int length);
    unsigned long _activityStartTime;
    const unsigned long ACTIVITY_DURATION = 100;

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
