#pragma once
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <WiFi.h>
#include <functional>
#include <string>
#include <vector>
#include "NetworkManagement.h"
#include "Config.h"

class MQTTManagement
{
private:
    NetworkManagement &_connection;
    AsyncMqttClient _mqttClient;
    const IPAddress _host = MQTT_HOST;
    const char *_username = MQTT_USER;
    const char *_password = MQTT_PASS;
    int _port = MQTT_PORT;
    String _clientId;
    const char *_willTopic = "device/status";
    std::vector<String> _topicsToSubscribe = {"environment/refresh",
                                              "pump/automation",
                                              "pump/manually"};
    unsigned long _activityStartTime = 0;
    const unsigned long ACTIVITY_DURATION = 3000;
    void _onMqttConnect(bool sessionPresent);
    void _onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    void _onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
    void _onMqttPublish(uint16_t packetId);

public:
    std::function<void(String, String)> messageReceivedCallback;
    std::function<void(bool)> statusCallback;
    MQTTManagement(NetworkManagement &connection);
    void setup();
    void loop();
    bool isConnected();
    bool publish(const char *topic, const char *payload, bool retain = false);
    bool subscribe(const char *topic);
    void reconnect();
};
