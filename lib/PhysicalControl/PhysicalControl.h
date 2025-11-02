#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <functional>
#include <vector>
#include <string>
#include "Config.h"
#include "NetworkManagement.h"
#include "MQTTManagement.h"
#include "MainTank.h"
#include "PHCorrector.h"
#include "TDSCorrector.h"

class PhysicalControl
{
private:
    unsigned long _lastHeartbeatToggle;
    bool _heartbeatLedState;
    NetworkManagement _networkManagement;
    MQTTManagement _MQTTManagement;
    MainTank _mainTank;
    PHCorrector _PHCorrector;
    TDSCorrector _TDSCorrector;
    std::vector<String> _topics;
    void _messageHandle(String topic, String payload);
    void _WifiStatusHandle(bool status);
    void _iternetStatusHandle(bool status);
    void _MQTTStatusHandle(bool status);
    void _refreshEnvHandle(String payload);
    void _activatePumpHandle(String payload);
    void _readEnvronmentHandle();

public:
    PhysicalControl();
    void setup();
    void loop();
};