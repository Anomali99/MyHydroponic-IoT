#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_MCP23X17.h>
#include <Wire.h>
#include <functional>
#include <queue>
#include <string>
#include <vector>
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

enum SourceType
{
    AUTOMATION,
    MANUALLY,
    LOCALLY
};

struct PumpData
{
    float duration;
    PumpType type;
    SourceType source;
    String datetime;
};

enum StatusState
{
    SETUP,
    IDLE,
    ENV_START,
    ENV_MIXING,
    ENV_READING,
    ENV_SENDING,
    PUMP_START,
    PUMP_ADDING,
    PUMP_SENDING,
};

class DeviceIoT
{
private:
    StatusState _statusState = SETUP;
    float _setupDurationTimeout = 120 * 1000;
    float _durationActivatePump = 0.5;
    long _debounce = 1000;
    unsigned long _lastWarning = 0;
    unsigned long _lastSetupTime = 0;
    bool _warningState = false;
    int _tankIndex = 0;
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
    std::queue<PumpData> _queuePump;
    bool _isIdle();
    void _buttonsHandle();
    void _heartbeatHandle();
    void _warningHandle();
    void _refreshLedState();
    void _mainProccessHandle();
    void _WifiStatusHandle(bool status);
    void _MQTTStatusHandle(bool status);
    void _MQTTMessageHandle(String &topic, String payload);
    void _refreshEnvHandle(String payload);
    void _activatePumpHandle(String payload);
    void _startReadEnvironment();
    void _startActivatePump(PumpType type, float duration, SourceType source);
    void _sendTankNotificationHandle(String type, float volume);
    void _resendPumpEnvHandle();
    String _pumpTypeToString(PumpType type);

public:
    DeviceIoT();
    void setup();
    void loop();
};