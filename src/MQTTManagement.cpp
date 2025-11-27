#include "MQTTManagement.h"

MQTTManagement::MQTTManagement(NetworkManagement &connection) : _connection(connection),
                                                                _mqttClient()
{
    _clientId = "ESP32Client-";
    _clientId += String(random(0xffff), HEX);
    messageReceivedCallback = nullptr;
    statusCallback = nullptr;
}

void MQTTManagement::setup()
{
    _mqttClient.onConnect([this](bool sessionPresent)
                          { this->_onMqttConnect(sessionPresent); });

    _mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason)
                             { this->_onMqttDisconnect(reason); });

    _mqttClient.onMessage([this](char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
                          { this->_onMqttMessage(topic, payload, properties, len, index, total); });

    _mqttClient.onPublish([this](uint16_t packetId)
                          { this->_onMqttPublish(packetId); });

    _mqttClient.setServer(_host, _port);

    _mqttClient.setCredentials(_username, _password);

    _mqttClient.setWill(_willTopic, 1, true, "0");

    if (statusCallback)
    {
        statusCallback(false);
    }

    Serial.println("--- MQTT Setup ---");
    Serial.printf("Broker Host: %s, Port: %d\n", MQTT_HOST, _port);
    Serial.printf("Client ID: %s\n", _clientId.c_str());
    Serial.println("------------------");
}

void MQTTManagement::loop()
{
    static unsigned long lastConnectionCheck = 0;
    static unsigned long lastBlink = 0;
    static unsigned long blinkState = 0;
    unsigned long now = millis();

    if (now - _activityStartTime < ACTIVITY_DURATION)
    {
        if (now - lastBlink > 500)
        {
            lastBlink = now;
            blinkState = blinkState == 0 ? 1 : 0;
            if (statusCallback)
                statusCallback(blinkState == 1 ? HIGH : LOW);
        }
    }
    else
    {
        if (now - lastConnectionCheck > 5000 || blinkState == 0)
        {
            blinkState = 1;
            lastConnectionCheck = now;
            if (!_mqttClient.connected())
            {
                if (statusCallback)
                    statusCallback(false);
                reconnect();
            }
            else
            {
                statusCallback(true);
            }
        }
    }
}

void MQTTManagement::reconnect()
{
    if (!_connection.isConnected())
    {
        if (statusCallback)
            statusCallback(false);
        return;
    }

    if (!_mqttClient.connected())
    {
        _mqttClient.connect();
    }
}

bool MQTTManagement::publish(const char *topic, const char *payload, bool retain)
{
    if (_mqttClient.connected())
    {
        _activityStartTime = millis();
        uint16_t packetId = _mqttClient.publish(topic, 1, retain, payload);

        if (packetId > 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool MQTTManagement::subscribe(const char *topic)
{
    if (_mqttClient.connected())
    {
        uint16_t packetId = _mqttClient.subscribe(topic, 1);
        if (packetId > 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void MQTTManagement::_onMqttConnect(bool sessionPresent)
{
    publish(_willTopic, "1", true);

    for (const String &topic : _topicsToSubscribe)
        subscribe(topic.c_str());

    if (statusCallback)
        statusCallback(true);
}

void MQTTManagement::_onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    if (statusCallback)
        statusCallback(false);
}

void MQTTManagement::_onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    _activityStartTime = millis();

    String payloadStr;
    payloadStr.reserve(len + 1);
    for (size_t i = 0; i < len; i++)
    {
        payloadStr += payload[i];
    }

    if (messageReceivedCallback)
        messageReceivedCallback(String(topic), payloadStr);
}

void MQTTManagement::_onMqttPublish(uint16_t packetId)
{
    _activityStartTime = millis();
}
