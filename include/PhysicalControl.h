#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_ADS1X15.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <functional>
#include <vector>
#include <string>
#include "LCDDisplay.h"
#include "NetworkManagement.h"
#include "MQTTManagement.h"
#include "MainTank.h"
#include "MeasuringTank.h"
#include "PHCorrector.h"
#include "TDSCorrector.h"
#include "InterruptButton.h"

enum PumpType
{
    NUTRITION,
    PH_UP,
    PH_DOWN
};

struct PumpData
{
    float duration;
    PumpType type;
    String datetime;
};

enum StatusState
{
    IDLE,
    READ_ENV,
    ADD_CORRECTOR
};

class PhysicalControl
{
private:
    StatusState _status;
    Adafruit_MCP23X17 _mcp;
    Adafruit_ADS1115 _ads;
    LCDDisplay _display;
    NetworkManagement _networkManagement;
    MQTTManagement _MQTTManagement;
    MainTank _mainTank;
    MeasuringTank _measuringTank;
    PHCorrector _PHCorrector;
    TDSCorrector _TDSCorrector;
    InterruptButton _btnReconnect;
    InterruptButton _btnReadEnv;
    InterruptButton _btnNutrition;
    InterruptButton _btnPhUp;
    InterruptButton _btnPhDown;
    std::vector<EnvData> _pendingEnv;
    std::vector<PumpData> _pendingPump;
    float _durationActivatePump = 10;
    long _debounce = 1000;
    bool _isIdle();
    void _buttonsHandle();
    void _heartbeatHandle();
    void _warningHandle();
    void _messageHandle(String topic, String payload);
    void _WifiStatusHandle(bool status);
    void _MQTTStatusHandle(bool status);
    void _refreshEnvHandle(String payload);
    void _activatePumpHandle(String payload);
    void _readEnvironmentHandle();
    void _pumpHandle(PumpType type);

public:
    PhysicalControl();
    void setup();
    void loop();
};