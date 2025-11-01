#include "PhysicalControl.h"

PhysicalControl::PhysicalControl() : _lastHeartbeatToggle(0),
                                     _heartbeatLedState(LOW),
                                     _topics({"environment/refresh", "pump/automation", "pump/manually"}),
                                     _networkManagement(NTP_SERVER, GMTOFFSET),
                                     _MQTTManagement(_networkManagement, MQTT_BROKER, MQTT_PORT, _topics),
                                     _mainTank(MT_US_TRIG_PIN, MT_US_ECHO_PIN, MT_VALVE_PIN, MT_MIXER_PIN, tankHeight, minLevel, maxLevel),
                                     _PHCorrector(PH_UP_US_TRIG_PIN, PH_UP_US_ECHO_PIN, PH_DOWN_US_TRIG_PIN, PH_DOWN_US_ECHO_PIN, PH_UP_PUMP_PIN, PH_DOWN_PUMP_PIN, phUpTankHeight, phDownTankHeight),
                                     _TDSCorrector(TDS_A_US_TRIG_PIN, TDS_A_US_ECHO_PIN, TDS_B_US_TRIG_PIN, TDS_B_US_ECHO_PIN, TDS_TDS_PUMP_PIN, ATankHeight, BTankHeight)
{
    _networkManagement.statusCallback = [this](bool status)
    {
        this->_WifiStatusHandle(status);
    };

    _networkManagement.statusInternetCallback = [this](bool status)
    {
        this->_iternetStatusHandle(status);
    };

    _MQTTManagement.statusCallback = [this](bool status)
    {
        this->_MQTTStatusHandle(status);
    };

    _MQTTManagement.messageReceivedCallback = [this](String topic, String payload)
    {
        this->_messageHandle(topic, payload);
    };
}

void PhysicalControl::setup()
{
    pinMode(PC_LED_HEARTBEAT_PIN, OUTPUT);
    pinMode(PC_LED_WIFI_PIN, OUTPUT);
    pinMode(PC_LED_INTERNET_PIN, OUTPUT);
    pinMode(PC_LED_MQTT_PIN, OUTPUT);
    pinMode(PC_LED_READ_ENV_PIN, OUTPUT);
    pinMode(PC_LED_ADD_EVENT_PIN, OUTPUT);
    pinMode(PC_BUZZER_PIN, OUTPUT);

    _networkManagement.setup(WIFI_SSID, WIFI_PASS);
    _MQTTManagement.setup();
    _mainTank.setup();
    _PHCorrector.setup();
    _TDSCorrector.setup();
}

void PhysicalControl::loop()
{
    _networkManagement.loop();
    _MQTTManagement.loop();

    static unsigned long lastReconnectAttempt = 0;
    unsigned long now = millis();
    if (now - _lastHeartbeatToggle > 500)
    {
        _lastHeartbeatToggle = now;
        _heartbeatLedState = !_heartbeatLedState;
        digitalWrite(PC_LED_HEARTBEAT_PIN, _heartbeatLedState ? HIGH : LOW);
    }
    lastReconnectAttempt = millis();
}

void PhysicalControl::_messageHandle(String topic, String payload)
{
    if (topic == "environment/refresh")
        _MQTTRefreshEnvHandle(payload);
    else if (topic == "pump/automation")
        _MQTTPumpAutoHandle(payload);
    else if (topic == "pump/manually")
        _MQTTPumpManuallyHandle(payload);
}

void PhysicalControl::_WifiStatusHandle(bool status)
{
    if (status)
    {
        digitalWrite(PC_LED_WIFI_PIN, HIGH);
    }
    else
    {
        digitalWrite(PC_LED_WIFI_PIN, LOW);
    }
}

void PhysicalControl::_iternetStatusHandle(bool status)
{
    if (status)
    {
        digitalWrite(PC_LED_INTERNET_PIN, HIGH);
    }
    else
    {
        digitalWrite(PC_LED_INTERNET_PIN, LOW);
    }
}

void PhysicalControl::_MQTTStatusHandle(bool status)
{
    if (status)
    {
        digitalWrite(PC_LED_MQTT_PIN, HIGH);
    }
    else
    {
        digitalWrite(PC_LED_MQTT_PIN, LOW);
    }
}

void PhysicalControl::_MQTTRefreshEnvHandle(String pyload) {}

void PhysicalControl::_MQTTPumpAutoHandle(String pyload) {}

void PhysicalControl::_MQTTPumpManuallyHandle(String pyload) {}

void PhysicalControl::_readEnvironmentHandle() {}

void PhysicalControl::_activateNutrientPump(float duration) {}

void PhysicalControl::_activatePhUpPump(float duration) {}

void PhysicalControl::_activatePhDownPump(float duration) {}
