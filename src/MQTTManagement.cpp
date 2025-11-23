#include "MQTTManagement.h"
#include "Config.h"

MQTTManagement::MQTTManagement(NetworkManagement &connection) : _connection(connection),
                                                                _mqttClient(connection.getClient()),
                                                                _broker(MQTT_BROKER),
                                                                _username(MQTT_USER),
                                                                _password(MQTT_PASS),
                                                                _port(MQTT_PORT),
                                                                _topicsToSubscribe({"environment/refresh",
                                                                                    "pump/automation",
                                                                                    "pump/manually"})
{
    _clientId = "ESP32Client-";
    _clientId += String(random(0xffff), HEX);
    _activityStartTime = 0;
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
    _activityStartTime = millis();

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
    static unsigned long lastConnectionCheck = 0;
    unsigned long now = millis();

    if (now - lastConnectionCheck > 5000)
    {
        lastConnectionCheck = now;
        if (!_mqttClient.connected())
        {
            if (statusCallback)
                statusCallback(false);
            reconnect();
        }
        else
        {
            if (statusCallback && (now - _activityStartTime > ACTIVITY_DURATION))
            {
                statusCallback(true);
            }
        }
    }

    if (_mqttClient.connected())
    {
        if (now - _activityStartTime < ACTIVITY_DURATION)
        {
            if (statusCallback)
            {
                statusCallback(false);
                statusCallback(true);
            }
        }
        else if (now - lastConnectionCheck > 5000)
        {
            if (statusCallback)
                statusCallback(true);
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

    if (_mqttClient.connect(_clientId.c_str(), _username, _password,
                            _willTopic, 1, true, _willPayload))
    {
        if (statusCallback)
            statusCallback(true);

        publish(_willTopic, "1", true);

        for (const String &topic : _topicsToSubscribe)
            subscribe(topic.c_str());
    }
    else
    {
        if (statusCallback)
            statusCallback(false);
    }
}

bool MQTTManagement::publish(const char *topic, const char *payload, bool retain)
{
    if (_mqttClient.connected())
        return _mqttClient.publish(topic, payload, retain);

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