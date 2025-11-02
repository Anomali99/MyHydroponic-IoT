#include "NetworkManagement.h"

NetworkManagement::NetworkManagement(
    const char *NTP_SERVER,
    const long GMTOFFSET,
    const int DAYLIGHTOFFSET_SEC) : NTP_SERVER(NTP_SERVER),
                                    GMTOFFSET_SEC(GMTOFFSET * 3600),
                                    DAYLIGHTOFFSET_SEC(DAYLIGHTOFFSET_SEC)
{
    _lastConnectionCheck = 0;
    statusCallback = nullptr;
    statusInternetCallback = nullptr;
}

void NetworkManagement::setup(const char *ssid, const char *pass)
{
    WiFi.begin(ssid, pass);

    if (statusCallback)
        statusCallback(false);

    if (statusInternetCallback)
        statusInternetCallback(false);
}

void NetworkManagement::loop()
{
    static unsigned long lastReconnectAttempt = 0;
    unsigned long now = millis();
    if (now - _lastConnectionCheck > 5000)
    {
        _lastConnectionCheck = now;
        if (!isConnected())
        {
            if (statusCallback)
                statusCallback(false);
            reconnect();
        }
        else
            checkInternetConnection();
    }
    lastReconnectAttempt = millis();
}

void NetworkManagement::checkInternetConnection()
{
    IPAddress result;
    bool currentInternetState = (WiFi.hostByName("google.com", result) == 1);
    if (statusInternetCallback)
    {
        statusInternetCallback(currentInternetState);
    }
}

void NetworkManagement::reconnect()
{
    if (WiFi.reconnect())
    {
        if (statusCallback)
        {
            statusCallback(true);
            checkInternetConnection();
        }
    }
    else
    {
        if (statusCallback)
        {
            statusCallback(false);
            statusInternetCallback(false);
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

void NetworkManagement::getHttpRequest(String url)
{
    if (!isConnected())
        return;

    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK)
        {
            String payload = http.getString();
        }
    }

    http.end();
}