#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
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
    void _MQTTRefreshEnvHandle(String payload);
    void _MQTTPumpAutoHandle(String payload);
    void _MQTTPumpManuallyHandle(String payload);
    void _readEnvironmentHandle();
    void _activateNutrientPump(float duration);
    void _activatePhUpPump(float duration);
    void _activatePhDownPump(float duration);

public:
    PhysicalControl();
    void setup();
    void loop();
};