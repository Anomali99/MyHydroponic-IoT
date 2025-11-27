#include "DeviceIoT.h"
#include "Config.h"

DeviceIoT::DeviceIoT() : _mcp(),
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
        this->_MQTTMessageHandle(topic, payload);
    };
}

void DeviceIoT::setup()
{
    _ads.begin(0x48);
    _ads.setGain(GAIN_TWOTHIRDS);
    _mcp.begin_I2C(0x20);

    _mcp.pinMode(PC_LED_HEARTBEAT_PIN, OUTPUT);
    _mcp.pinMode(PC_LED_WIFI_PIN, OUTPUT);
    _mcp.pinMode(PC_LED_WARNING_PIN, OUTPUT);
    _mcp.pinMode(PC_LED_MQTT_PIN, OUTPUT);
    _mcp.pinMode(PC_LED_READ_ENV_PIN, OUTPUT);
    _mcp.pinMode(PC_LED_ADD_EVENT_PIN, OUTPUT);
    _mcp.pinMode(PC_BUZZER_PIN, OUTPUT);

    _mcp.digitalWrite(PC_BUZZER_PIN, LOW);

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

    _startReadEnvironment();
}

void DeviceIoT::loop()
{
    _networkManagement.loop();
    _MQTTManagement.loop();
    _mainTank.loop();
    _PHCorrector.loop();
    _TDSCorrector.loop();
    _display.loop();

    _buttonsHandle();
    _heartbeatHandle();
    _warningHandle();
    _mainProccessHandle();
}

bool DeviceIoT::_isIdle()
{
    if (_statusState != IDLE)
    {
        _mcp.digitalWrite(PC_BUZZER_PIN, HIGH);
        delay(200);
        _mcp.digitalWrite(PC_BUZZER_PIN, LOW);

        return false;
    }

    return true;
}

void DeviceIoT::_heartbeatHandle()
{
    static unsigned long lastHeartbeat = 0;
    static unsigned long heartbeatState = 0;
    unsigned long now = millis();

    if (now - lastHeartbeat > 500)
    {
        lastHeartbeat = now;
        heartbeatState = heartbeatState == 0 ? 1 : 0;
        _mcp.digitalWrite(PC_LED_HEARTBEAT_PIN, heartbeatState == 1 ? HIGH : LOW);
    }
}

void DeviceIoT::_warningHandle()
{
    static unsigned long lastWarning = 0;
    static unsigned long warningState = 0;

    if (_mainTank.isWarning(), _PHCorrector.isWarning(), _TDSCorrector.isWarning() || warningState)
    {
        unsigned long now = millis();

        if (now - lastWarning > 500)
        {
            lastWarning = now;
            warningState = warningState == 0 ? 1 : 0;
            _mcp.digitalWrite(PC_LED_WARNING_PIN, warningState == 1 ? HIGH : LOW);
            _mcp.digitalWrite(PC_BUZZER_PIN, warningState == 1 ? HIGH : LOW);
        }
    }
}

void DeviceIoT::_buttonsHandle()
{
    _btnReconnect.update();
    _btnReadEnv.update();
    _btnNutrition.update();
    _btnPhUp.update();
    _btnPhDown.update();

    unsigned long now = millis();
    bool anyButtonPressed = _btnReconnect.isBtnPressed() || _btnReadEnv.isBtnPressed() || _btnNutrition.isBtnPressed() || _btnPhUp.isBtnPressed() || _btnPhDown.isBtnPressed();

    if (anyButtonPressed)
    {

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
            _startReadEnvironment();
        }

        if (_btnNutrition.isBtnPressed() && (now - _btnNutrition.getLastPress() > _debounce))
        {
            _btnNutrition.setBtnPressed(false);
            _btnNutrition.setLastPress(now);
            _startActivatePump(NUTRITION, _durationActivatePump);
            ;
        }

        if (_btnPhUp.isBtnPressed() && (now - _btnPhUp.getLastPress() > _debounce))
        {
            _btnPhUp.setBtnPressed(false);
            _btnPhUp.setLastPress(now);
            _startActivatePump(PH_UP, _durationActivatePump);
        }

        if (_btnPhDown.isBtnPressed() && (now - _btnPhDown.getLastPress() > _debounce))
        {
            _btnPhDown.setBtnPressed(false);
            _btnPhDown.setLastPress(now);
            _startActivatePump(PH_DOWN, _durationActivatePump);
        }
    }
}

void DeviceIoT::_WifiStatusHandle(bool status)
{
    _mcp.digitalWrite(PC_LED_WIFI_PIN, status);
}

void DeviceIoT::_MQTTStatusHandle(bool status)
{
    _mcp.digitalWrite(PC_LED_MQTT_PIN, status);
}

void DeviceIoT::_MQTTMessageHandle(String topic, String payload)
{

    if (topic == "environment/refresh")
    {
        if (payload == "1")
        {
            _startReadEnvironment();
        }
    }
    else if (topic == "pump/automation" || topic == "pump/manually")
    {

        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            return;
        }

        if (doc.containsKey("pump") && doc.containsKey("duration"))
        {
            String pump = doc["pump"].as<String>();
            float duration = doc["duration"].as<float>();

            if (pump == "NUTRITION")
            {
                _startActivatePump(NUTRITION, duration);
            }
            else if (pump == "PH_UP")
            {
                _startActivatePump(PH_UP, duration);
            }
            else if (pump == "PH_DOWN")
            {
                _startActivatePump(PH_DOWN, duration);
            }
        }
    }
}

void DeviceIoT::_mainProccessHandle()
{
    switch (_statusState)
    {
    case IDLE:
    {
        break;
    }

    case ENV_MIXING:
    {
        if (!_mainTank.isActiveMixer())
        {
            _statusState = ENV_READING;
            _measuringTank.startReadData();
        }
        break;
    }

    case ENV_READING:
    {
        MeasuringState measuringState = _measuringTank.getState();

        if (measuringState == CLEAN_TANK || measuringState == MEASURING_IDLE)
        {
            EnvironmentData env = _measuringTank.getData();

            EnvData envData;
            envData.ph = env.ph;
            envData.tds = env.tds;
            envData.temp = env.temp;
            envData.tankA = _TDSCorrector.getACurrentVolume();
            envData.tankB = _TDSCorrector.getBCurrentVolume();
            envData.tankPhUp = _PHCorrector.getPhUpCurrentVolume();
            envData.tankPhDown = _PHCorrector.getPhDownCurrentVolume();
            envData.tankMain = _mainTank.getCurrentVolume();
            envData.datetime = _networkManagement.getCurrentTime();

            _pendingEnv.push_back(envData);
            _display.showData(envData);

            _statusState = ENV_SENDING;
        }
        break;
    }

    case ENV_SENDING:
    {
        EnvData sendingEnvData = _pendingEnv.back();

        StaticJsonDocument<200> sendingEnvJson;
        sendingEnvJson["ph"] = sendingEnvData.ph;
        sendingEnvJson["tds"] = sendingEnvData.tds;
        sendingEnvJson["temp"] = sendingEnvData.temp;
        sendingEnvJson["tank_a"] = sendingEnvData.tankA;
        sendingEnvJson["tank_b"] = sendingEnvData.tankB;
        sendingEnvJson["tank_ph_up"] = sendingEnvData.tankPhUp;
        sendingEnvJson["tank_ph_down"] = sendingEnvData.tankPhDown;
        sendingEnvJson["tank_main"] = sendingEnvData.tankMain;
        sendingEnvJson["datetime"] = sendingEnvData.datetime;

        char buffer[200];
        serializeJson(sendingEnvJson, buffer);
        bool status = _MQTTManagement.publish("environment/data", buffer, true);

        if (status)
            _pendingEnv.pop_back();

        _mcp.digitalWrite(PC_LED_READ_ENV_PIN, LOW);
        _statusState = IDLE;
        break;
    }

    case PUMP_ADDING:
    {
        bool pumpStatus = _PHCorrector.isActivePump() || _TDSCorrector.isActivePump();
        if (!pumpStatus)
        {
            _mainTank.activeMixer();
            _statusState = IDLE;
        }
        break;
    }

    case PUMP_SENDING:
    {
        PumpData sendingPumpData = _pendingPump.back();

        StaticJsonDocument<200> sendingPumpJson;
        sendingPumpJson["duration"] = sendingPumpData.duration;
        sendingPumpJson["pump"] = sendingPumpData.type;
        sendingPumpJson["datetime"] = sendingPumpData.datetime;

        char buffer[200];
        serializeJson(sendingPumpJson, buffer);
        bool status = _MQTTManagement.publish("pump/locally", buffer);

        if (status)
            _pendingPump.pop_back();

        _statusState = IDLE;
        _mcp.digitalWrite(PC_LED_ADD_EVENT_PIN, LOW);
        _startReadEnvironment();
        break;
    }
    }
}

void DeviceIoT::_startReadEnvironment()
{
    if (_statusState == IDLE)
    {
        _mcp.digitalWrite(PC_LED_READ_ENV_PIN, HIGH);
        _statusState = ENV_MIXING;
        _mainTank.activeMixer();
    }
}

void DeviceIoT::_startActivatePump(PumpType type, float duration)
{
    if (_statusState == IDLE)
    {
        _mcp.digitalWrite(PC_LED_ADD_EVENT_PIN, HIGH);
        _statusState = PUMP_ADDING;

        switch (type)
        {
        case NUTRITION:
            _TDSCorrector.activePump(duration);
            break;
        case PH_UP:
            _PHCorrector.activePhUpPump(duration);
            break;
        case PH_DOWN:
            _PHCorrector.activePhDownPump(duration);
            break;
        }

        PumpData data;
        data.duration = duration;
        data.type = type;
        data.datetime = _networkManagement.getCurrentTime();

        _pendingPump.push_back(data);
    }
}

void DeviceIoT::_resendPumpEnvHandle() {}