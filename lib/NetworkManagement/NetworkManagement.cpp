#include "NetworkManagement.h"

NetworkManagement::NetworkManagement() {}

void NetworkManagement::setup(const char *ssid, const char *pass)
{
    WiFi.begin(ssid, pass);
}

void NetworkManagement::loop()
{
    if (!isConnected())
    {
        static unsigned long lastReconnectAttempt = 0;
        if (millis() - lastReconnectAttempt < 5000)
        {
            if (statusCallback)
            {
                statusCallback(false);
            }
            return;
        }
        lastReconnectAttempt = millis();
        reconnect();
    }
}

void NetworkManagement::reconnect()
{
    if (WiFi.reconnect())
    {
        if (statusCallback)
        {
            statusCallback(true);
        }
    }
    else
    {
        if (statusCallback)
        {
            statusCallback(true);
        }
    }
}

bool NetworkManagement::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

WiFiClient &NetworkManagement::getClient()
{
    return _wifiClient;
}