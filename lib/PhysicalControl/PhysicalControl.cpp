#include "PhysicalControl.h"

PhysicalControl::PhysicalControl() : _lastHeartbeatToggle(0),
                                     _heartbeatLedState(LOW),
                                     _display(0x27),
                                     _topics({"environment/refresh", "pump/automation", "pump/manually"}),
                                     _networkManagement(NTP_SERVER, GMTOFFSET),
                                     _MQTTManagement(_networkManagement, MQTT_BROKER, MQTT_PORT, _topics),
                                     _mainTank(MT_US_TRIG_PIN, MT_US_ECHO_PIN, MT_VALVE_PIN, MT_MIXER_PIN, tankHeight, minLevel, maxLevel),
                                     _measuringTank(MeT_PH_PIN, MeT_TDS_PIN, MeT_VALVE_PIN, MeT_PUMP_PIN),
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

    _display.setup();
    _networkManagement.setup(WIFI_SSID, WIFI_PASS);
    _MQTTManagement.setup();
    _mainTank.setup();
    _measuringTank.setup();
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
        _refreshEnvHandle(payload);

    else if (topic == "pump/automation")
        _activatePumpHandle(payload);

    else if (topic == "pump/manually")
        _activatePumpHandle(payload);
}

void PhysicalControl::_WifiStatusHandle(bool status)
{
    if (status)
        digitalWrite(PC_LED_WIFI_PIN, HIGH);

    else
        digitalWrite(PC_LED_WIFI_PIN, LOW);
}

void PhysicalControl::_iternetStatusHandle(bool status)
{
    if (status)
        digitalWrite(PC_LED_INTERNET_PIN, HIGH);

    else
        digitalWrite(PC_LED_INTERNET_PIN, LOW);
}

void PhysicalControl::_MQTTStatusHandle(bool status)
{
    if (status)
        digitalWrite(PC_LED_MQTT_PIN, HIGH);

    else
        digitalWrite(PC_LED_MQTT_PIN, LOW);
}

void PhysicalControl::_refreshEnvHandle(String payload)
{
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
        return;

    if (doc.containsKey("url"))
    {
        JsonVariant urlVariant = doc["url"];

        if (urlVariant.is<const char *>())
        {
            String url = urlVariant.as<String>();
            _networkManagement.getHttpRequest(url);
        }
    }

    if (doc.containsKey("status"))
    {
        bool status = doc["status"].as<bool>();

        if (status)
            _readEnvronmentHandle();
    }
}

void PhysicalControl::_activatePumpHandle(String payload)
{
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
        return;

    if (doc.containsKey("pump") && doc.containsKey("duration"))
    {
        String pump = doc["pump"].as<String>();
        float duration = doc["duration"].as<float>();

        if (pump == "NUTRITION")
            _TDSCorrector.activePump(duration);

        else if (pump == "PH_UP")
            _PHCorrector.activePhUpPump(duration);

        else if (pump == "PH_DOWN")
            _PHCorrector.activePhDownPump(duration);
    }
}

void PhysicalControl::_readEnvronmentHandle()
{
    float mainTankLevel = _mainTank.getLevelCm();
    float phUpLevel = _PHCorrector.getPhUpLevelCm();
    float phDownLevel = _PHCorrector.getPhDownLevelCm();
    float nutrientALevel = _TDSCorrector.getALevelCm();
    float nutrientBLevel = _TDSCorrector.getBLevelCm();
    EnvironmentData env = _measuringTank.readData();
    String datetime = _networkManagement.getCurrentTime();

    StaticJsonDocument<200> json;
    json["ph"] = env.ph;
    json["tds"] = env.tds;
    json["tank_a"] = nutrientALevel;
    json["tank_b"] = nutrientBLevel;
    json["tank_ph_up"] = phUpLevel;
    json["tank_down_up"] = phDownLevel;
    json["tank_main"] = mainTankLevel;
    json["datetime"] = datetime;

    char buffer[200];
    serializeJson(json, buffer);
    _MQTTManagement.publish("environment/data", buffer);

    DisplayData data;
    data.ph = env.ph;
    data.tds = env.tds;
    data.mainTankLevel = mainTankLevel;
    data.nutrientALevel = nutrientALevel;
    data.nutrientBLevel = nutrientBLevel;
    data.phUpLevel = phUpLevel;
    data.phDownLevel = phDownLevel;
    data.datetime = datetime;

    _display.showData(data);
}