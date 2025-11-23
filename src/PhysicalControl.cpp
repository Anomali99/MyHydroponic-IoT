#include "PhysicalControl.h"
#include "Config.h"

PhysicalControl::PhysicalControl() : _mcp(),
                                     _ads(),
                                     _display(0x27),
                                     _networkManagement(),
                                     _MQTTManagement(_networkManagement),
                                     _mainTank(_mcp),
                                     _measuringTank(_ads, _mcp),
                                     _PHCorrector(_mcp),
                                     _TDSCorrector(_mcp),
                                     _btnReconnect(PC_BTN_RECONNECT_PIN),
                                     _btnReadEnv(PC_BTN_READ_ENV_PIN),
                                     _btnNutrition(PC_BTN_ADD_NUTRITION_PIN),
                                     _btnPhUp(PC_BTN_ADD_PH_UP_PIN),
                                     _btnPhDown(PC_BTN_ADD_PH_DOWN_PIN)
{
    _status = IDLE;

    _networkManagement.statusCallback = [this](bool status)
    {
        this->_WifiStatusHandle(status);
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
    _ads.begin();
    _ads.setGain(GAIN_TWOTHIRDS);
    _mcp.begin_I2C();

    _mcp.pinMode(PC_LED_HEARTBEAT_PIN, OUTPUT);
    _mcp.pinMode(PC_LED_WIFI_PIN, OUTPUT);
    _mcp.pinMode(PC_LED_WARNING_PIN, OUTPUT);
    _mcp.pinMode(PC_LED_MQTT_PIN, OUTPUT);
    _mcp.pinMode(PC_LED_READ_ENV_PIN, OUTPUT);
    _mcp.pinMode(PC_LED_ADD_EVENT_PIN, OUTPUT);
    _mcp.pinMode(PC_BUZZER_PIN, OUTPUT);

    _display.setup();
    _networkManagement.setup();
    _MQTTManagement.setup();
    _mainTank.setup();
    _measuringTank.setup();
    _PHCorrector.setup();
    _TDSCorrector.setup();

    _btnReconnect.setup();
    _btnReadEnv.setup();
    _btnNutrition.setup();
    _btnPhUp.setup();
    _btnPhDown.setup();
}

void PhysicalControl::loop()
{
    _networkManagement.loop();
    _MQTTManagement.loop();
    _mainTank.loop();

    _buttonsHandle();
    _heartbeatHandle();
    _warningHandle();
}

bool PhysicalControl::_isIdle()
{
    if (_status != IDLE)
    {
        _mcp.digitalWrite(PC_BUZZER_PIN, HIGH);
        delay(200);
        _mcp.digitalWrite(PC_BUZZER_PIN, HIGH);
    }

    return true;
}

void PhysicalControl::_heartbeatHandle()
{
    static unsigned long lastHeartbeat = 0;
    static unsigned long heartbeatState = false;
    unsigned long now = millis();

    if (now - lastHeartbeat > 500)
    {
        lastHeartbeat = now;
        heartbeatState = !heartbeatState;
        _mcp.digitalWrite(PC_LED_HEARTBEAT_PIN, heartbeatState ? HIGH : LOW);
    }
}

void PhysicalControl::_warningHandle()
{
    static unsigned long lastWarning = 0;
    static unsigned long warningState = false;

    if (_mainTank.isWarning(), _PHCorrector.isWarning(), _TDSCorrector.isWarning() || warningState)
    {
        unsigned long now = millis();

        if (now - lastWarning > 200)
        {
            lastWarning = now;
            warningState = !warningState;
            _mcp.digitalWrite(PC_LED_WARNING_PIN, warningState ? HIGH : LOW);
            _mcp.digitalWrite(PC_BUZZER_PIN, warningState ? HIGH : LOW);
        }
    }
}

void PhysicalControl::_buttonsHandle()
{
    unsigned long now = millis();
    bool anyButtonPressed = _btnReconnect.isBtnPressed() || _btnReadEnv.isBtnPressed() || _btnNutrition.isBtnPressed() || _btnPhUp.isBtnPressed() || _btnPhDown.isBtnPressed();

    if (anyButtonPressed)
    {
        noInterrupts();

        if (_btnReconnect.isBtnPressed() && (now - _btnReconnect.getLastPress() > _debounce))
        {
            _btnReconnect.setBtnPressed(false);
            _btnReconnect.setLastPress(now);
            _networkManagement.reconnect();
        }

        if (_btnReadEnv.isBtnPressed() && (now - _btnReadEnv.getLastPress() > _debounce))
        {
            _btnReadEnv.setBtnPressed(false);
            _btnReadEnv.setLastPress(now);
            _readEnvironmentHandle();
        }

        if (_btnNutrition.isBtnPressed() && (now - _btnNutrition.getLastPress() > _debounce))
        {
            _btnNutrition.setBtnPressed(false);
            _btnNutrition.setLastPress(now);
            _pumpHandle(NUTRITION);
            ;
        }

        if (_btnPhUp.isBtnPressed() && (now - _btnPhUp.getLastPress() > _debounce))
        {
            _btnPhUp.setBtnPressed(false);
            _btnPhUp.setLastPress(now);
            _pumpHandle(PH_UP);
        }

        if (_btnPhDown.isBtnPressed() && (now - _btnPhDown.getLastPress() > _debounce))
        {
            _btnPhDown.setBtnPressed(false);
            _btnPhDown.setLastPress(now);
            _pumpHandle(PH_DOWN);
        }

        interrupts();
    }
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
    _mcp.digitalWrite(PC_LED_WIFI_PIN, status);
}

void PhysicalControl::_MQTTStatusHandle(bool status)
{
    _mcp.digitalWrite(PC_LED_MQTT_PIN, status);
}

void PhysicalControl::_refreshEnvHandle(String payload)
{
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
        return;

    if (doc.containsKey("status"))
    {
        bool status = doc["status"].as<bool>();

        if (status)
            _readEnvironmentHandle();
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

void PhysicalControl::_readEnvironmentHandle()
{
    _mcp.digitalWrite(PC_LED_READ_ENV_PIN, HIGH);
    _status = READ_ENV;

    EnvironmentData env = _measuringTank.readData();

    EnvData data;
    data.ph = env.ph;
    data.tds = env.tds;
    data.temp = env.temp;
    data.tankA = _TDSCorrector.getACurrentVolume();
    data.tankB = _TDSCorrector.getBCurrentVolume();
    data.tankPhUp = _PHCorrector.getPhUpCurrentVolume();
    data.tankPhDown = _PHCorrector.getPhDownCurrentVolume();
    data.tankMain = _mainTank.getCurrentVolume();
    data.datetime = _networkManagement.getCurrentTime();

    StaticJsonDocument<200> json;
    json["ph"] = data.ph;
    json["tds"] = data.tds;
    json["temp"] = data.temp;
    json["tank_a"] = data.tankA;
    json["tank_b"] = data.tankB;
    json["tank_ph_up"] = data.tankPhUp;
    json["tank_ph_down"] = data.tankPhDown;
    json["tank_main"] = data.tankMain;
    json["datetime"] = data.datetime;

    char buffer[200];
    serializeJson(json, buffer);
    bool status = _MQTTManagement.publish("environment/data", buffer);

    if (!status)
        _pendingEnv.push_back(data);

    _display.showData(data);

    _status = IDLE;
    _mcp.digitalWrite(PC_LED_READ_ENV_PIN, LOW);
}

void PhysicalControl::_pumpHandle(PumpType type)
{
    if (_isIdle())
        return;

    _mcp.digitalWrite(PC_LED_ADD_EVENT_PIN, HIGH);
    _status = ADD_CORRECTOR;

    if (type == NUTRITION)
        _TDSCorrector.activePump(_durationActivatePump);
    else if (type == PH_UP)
        _PHCorrector.activePhUpPump(_durationActivatePump);
    else if (type == PH_DOWN)
        _PHCorrector.activePhDownPump(_durationActivatePump);

    PumpData data;
    data.duration = _durationActivatePump;
    data.type = type;
    data.datetime = _networkManagement.getCurrentTime();

    StaticJsonDocument<200> json;
    json["duration"] = data.duration;
    json["pump"] = data.type;
    json["datetime"] = data.datetime;

    char buffer[200];
    serializeJson(json, buffer);
    bool status = _MQTTManagement.publish("pump/locally", buffer);

    if (!status)
        _pendingPump.push_back(data);

    _status = IDLE;
    _mcp.digitalWrite(PC_LED_ADD_EVENT_PIN, LOW);
}
