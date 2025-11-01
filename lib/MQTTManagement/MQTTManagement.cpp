#include "MQTTManagement.h"

MQTTManagement::MQTTManagement(
    NetworkManagement &connection,
    const char *broker,
    int port,
    std::vector<String> topics) : _connection(connection),
                                  _mqttClient(connection.getClient()),
                                  _broker(broker),
                                  _port(port),
                                  _topicsToSubscribe(topics)
{
    _clientId = "ESP32Client-";
    _clientId += String(random(0xffff), HEX);
    _lastConnectionCheck = 0;
    messageReceivedCallback = nullptr;
    statusCallback = nullptr;
}

void MQTTManagement::setup()
{
    _mqttClient.setServer(_broker, _port);
    _mqttClient.setCallback([this](char *topic, byte *payload, unsigned int length)
                            { this->callback(topic, payload, length); });
}

void MQTTManagement::callback(char *topic, byte *payload, unsigned int length)
{
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';

    String strTopic = String(topic);
    String strMessage = String(message);

    if (messageReceivedCallback)
        messageReceivedCallback(strTopic, strMessage);
}

void MQTTManagement::loop()
{
    static unsigned long lastReconnectAttempt = 0;
    unsigned long now = millis();
    if (now - _lastConnectionCheck > 5000)
    {
        _lastConnectionCheck = now;
        if (!_mqttClient.connected())
        {
            if (statusCallback)
                statusCallback(false);
            reconnect();
        }
    }

    _mqttClient.loop();
}

void MQTTManagement::reconnect()
{
    if (!_connection.isConnected())
    {
        if (statusCallback)
            statusCallback(false);
        return;
    }

    if (_mqttClient.connect(_clientId.c_str()))
    {
        if (statusCallback)
            statusCallback(true);

        for (const String &topic : _topicsToSubscribe)
            subscribe(topic.c_str());
    }
    else
    {
        if (statusCallback)
            statusCallback(false);
    }
}

bool MQTTManagement::publish(const char *topic, const char *payload)
{
    if (_mqttClient.connected())
        return _mqttClient.publish(topic, payload);

    else
        return false;
}

bool MQTTManagement::subscribe(const char *topic)
{
    if (_mqttClient.connected())
        return _mqttClient.subscribe(topic);
    else
        return false;
}