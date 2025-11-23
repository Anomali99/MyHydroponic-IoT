#include "NetworkManagement.h"
#include "Config.h"

NetworkManagement::NetworkManagement()
{
    currentState = NONE;
    statusCallback = nullptr;
}

void NetworkManagement::setup()
{
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    currentState = CONNECTING;
}

void NetworkManagement::loop()
{
    static unsigned long lastConnectionCheck = 0;
    static unsigned long lastBlinkTime = 0;

    unsigned long now = millis();
    if (now - lastConnectionCheck > 5000)
    {
        lastConnectionCheck = now;

        if (isConnected())
        {
            if (currentState != CONNECTED)
            {
                currentState = CONNECTED;
                if (statusCallback)
                    statusCallback(true);
            }
        }
        else
        {
            if (currentState == CONNECTED)
            {
                currentState = DISCONNECTED;
                if (statusCallback)
                    statusCallback(false);
            }
            else if (currentState != CONNECTING)
            {
                currentState = CONNECTING;
                reconnect();
            }
        }
    }

    if (currentState == CONNECTING)
    {
        if (now - lastBlinkTime >= 250)
        {
            lastBlinkTime = now;

            if (statusCallback)
            {
                static bool ledStatus = false;
                ledStatus = !ledStatus;
                statusCallback(ledStatus);
            }
        }
    }
}

void NetworkManagement::reconnect()
{
    if (!WiFi.reconnect())
    {
        currentState = DISCONNECTED;
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

String NetworkManagement::getCurrentTime(const char *format)
{
    configTime(GMTOFFSET_SEC, DAYLIGHTOFFSET_SEC, NTP_SERVER);

    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        char timeString[64];
        strftime(timeString, sizeof(timeString), format, &timeinfo);
        return String(timeString);
    }
    else
        return "null";
}
