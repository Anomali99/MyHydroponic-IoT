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

    _mcp.digitalWrite(PC_LED_HEARTBEAT_PIN, LOW);
    _mcp.digitalWrite(PC_LED_WIFI_PIN, LOW);
    _mcp.digitalWrite(PC_LED_WARNING_PIN, LOW);
    _mcp.digitalWrite(PC_LED_MQTT_PIN, LOW);
    _mcp.digitalWrite(PC_LED_READ_ENV_PIN, LOW);
    _mcp.digitalWrite(PC_LED_ADD_EVENT_PIN, LOW);
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
}

void DeviceIoT::loop()
{
    _networkManagement.loop();
    _MQTTManagement.loop();
    _mainTank.loop();
    _measuringTank.loop();
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
        _lastWarning = millis();
        _warningState = !_warningState;
        _mcp.digitalWrite(PC_LED_WARNING_PIN, _warningState ? HIGH : LOW);
        _mcp.digitalWrite(PC_BUZZER_PIN, _warningState ? HIGH : LOW);

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
    static bool lastMainTankWarningStatus = false;

    bool currentMainTankWarning = _mainTank.isWarning();
    if (currentMainTankWarning && !lastMainTankWarningStatus)
    {
        _sendTankNotificationHandle("MAIN_TANK", _mainTank.getCurrentVolume());
    }
    lastMainTankWarningStatus = currentMainTankWarning;

    bool isGlobalWarning = _warningState || _mainTank.isWarning() || _PHCorrector.isUpWarning() || _PHCorrector.isDownWarning() || _TDSCorrector.isAWarning() || _TDSCorrector.isBWarning();

    if (isGlobalWarning)
    {
        unsigned long now = millis();
        if (now - _lastWarning > 500)
        {
            _lastWarning = now;
            _warningState = !_warningState;
            _mcp.digitalWrite(PC_LED_WARNING_PIN, _warningState ? HIGH : LOW);
            _mcp.digitalWrite(PC_BUZZER_PIN, _warningState ? HIGH : LOW);
        }
    }
    else
    {
        _mcp.digitalWrite(PC_LED_WARNING_PIN, LOW);
        _mcp.digitalWrite(PC_BUZZER_PIN, LOW);
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

            if (_statusState == SETUP)
            {
                _statusState = IDLE;
                _startReadEnvironment();
            }
            else
            {
                _networkManagement.reconnect();
            }
        }

        if (_btnReadEnv.isBtnPressed() && (now - _btnReadEnv.getLastPress() > _debounce))
        {
            _btnReadEnv.setBtnPressed(false);
            _btnReadEnv.setLastPress(now);

            if (_statusState == SETUP)
            {
                _tankIndex++;
                _lastSetupTime = now;
                if (_tankIndex >= 5)
                {
                    _tankIndex = 0;
                }
            }
            else if (_isIdle())
            {
                _startReadEnvironment();
            }
        }

        if (_btnNutrition.isBtnPressed() && (now - _btnNutrition.getLastPress() > _debounce))
        {
            _btnNutrition.setBtnPressed(false);
            _btnNutrition.setLastPress(now);

            if (_statusState == SETUP)
            {
                _tankIndex--;
                _lastSetupTime = now;
                if (_tankIndex < 0)
                {
                    _tankIndex = 4;
                }
            }
            else if (_isIdle())
            {
                _startActivatePump(NUTRITION, _durationActivatePump, LOCALLY);
            };
        }

        if (_btnPhUp.isBtnPressed() && (now - _btnPhUp.getLastPress() > _debounce))
        {
            _btnPhUp.setBtnPressed(false);
            _btnPhUp.setLastPress(now);

            if (_statusState != SETUP)
            {
                if (_isIdle())
                {
                    _startActivatePump(PH_UP, _durationActivatePump, LOCALLY);
                }
            }
        }

        if (_btnPhDown.isBtnPressed() && (now - _btnPhDown.getLastPress() > _debounce))
        {
            _btnPhDown.setBtnPressed(false);
            _btnPhDown.setLastPress(now);

            if (_statusState != SETUP)
            {
                if (_isIdle())
                {
                    _startActivatePump(PH_DOWN, _durationActivatePump, LOCALLY);
                }
            }
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

void DeviceIoT::_MQTTMessageHandle(String &topic, String payload)
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
        SourceType source = topic == "pump/automation" ? AUTOMATION : MANUALLY;

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
                _startActivatePump(NUTRITION, duration, source);
            }
            else if (pump == "PH_UP")
            {
                _startActivatePump(PH_UP, duration, source);
            }
            else if (pump == "PH_DOWN")
            {
                _startActivatePump(PH_DOWN, duration, source);
            }
        }
    }
}

void DeviceIoT::_mainProccessHandle()
{
    long now = millis();
    switch (_statusState)
    {
    case SETUP:
    {
        static unsigned long lastSetup = 0;
        if (now - lastSetup > 100)
        {
            SetupData setupData;
            if (_tankIndex == 4)
            {
                setupData = _TDSCorrector.getBSetupData();
            }
            else if (_tankIndex == 3)
            {
                setupData = _TDSCorrector.getASetupData();
            }
            else if (_tankIndex == 2)
            {
                setupData = _PHCorrector.getPhDownSetupData();
            }
            else if (_tankIndex == 1)
            {
                setupData = _PHCorrector.getPhUpSetupData();
            }
            else
            {
                setupData = _mainTank.getSetupData();
            }

            _display.showSetupData(setupData);
        }

        if (now - _lastSetupTime > _setupDurationTimeout)
        {
            _statusState = IDLE;
            _startReadEnvironment();
        }
        break;
    }

    case IDLE:
    {
        static unsigned long lastResend = 0;
        if (!_queuePump.empty())
        {
            _statusState = PUMP_START;
        }
        else if (now - lastResend > 60000 && _MQTTManagement.isConnected())
        {
            _resendPumpEnvHandle();
            lastResend = now;
        }
        break;
    }

    case ENV_START:
    {
        _mcp.digitalWrite(PC_LED_READ_ENV_PIN, HIGH);
        _statusState = ENV_MIXING;
        _mainTank.activeMixer();
        break;
    }

    case ENV_MIXING:
    {
        if (!_mainTank.isActiveMixer())
        {
            _measuringTank.startReadData();
            _statusState = ENV_READING;
        }
        break;
    }

    case ENV_READING:
    {
        MeasuringState measuringState = _measuringTank.getState();

        if (measuringState == MEASURING_IDLE || measuringState == CLEAN_TANK)
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

        StaticJsonDocument<512> sendingEnvJson;
        sendingEnvJson["ph"] = sendingEnvData.ph;
        sendingEnvJson["tds"] = sendingEnvData.tds;
        sendingEnvJson["temp"] = sendingEnvData.temp;
        sendingEnvJson["tank_a"] = sendingEnvData.tankA;
        sendingEnvJson["tank_b"] = sendingEnvData.tankB;
        sendingEnvJson["tank_ph_up"] = sendingEnvData.tankPhUp;
        sendingEnvJson["tank_ph_down"] = sendingEnvData.tankPhDown;
        sendingEnvJson["tank_main"] = sendingEnvData.tankMain;
        sendingEnvJson["datetime"] = sendingEnvData.datetime;

        char buffer[512];
        serializeJson(sendingEnvJson, buffer);
        bool status = _MQTTManagement.publish("environment/data", buffer, true);

        if (status)
        {
            _pendingEnv.pop_back();
        }

        if (_PHCorrector.isUpWarning())
        {
            _sendTankNotificationHandle("PH_UP", sendingEnvData.tankPhUp);
        }

        if (_PHCorrector.isDownWarning())
        {
            _sendTankNotificationHandle("PH_DOWN", sendingEnvData.tankPhDown);
        }

        if (_TDSCorrector.isAWarning())
        {
            _sendTankNotificationHandle("NUTRITION_A", sendingEnvData.tankA);
        }

        if (_TDSCorrector.isBWarning())
        {
            _sendTankNotificationHandle("NUTRITION_B", sendingEnvData.tankB);
        }

        _mcp.digitalWrite(PC_LED_READ_ENV_PIN, LOW);
        _statusState = IDLE;
        break;
    }

    case PUMP_START:
    {
        _mcp.digitalWrite(PC_LED_ADD_EVENT_PIN, HIGH);

        PumpData queuePumpData = _queuePump.front();

        switch (queuePumpData.type)
        {
        case NUTRITION:
            _TDSCorrector.activePump(queuePumpData.duration);
            break;
        case PH_UP:
            _PHCorrector.activePhUpPump(queuePumpData.duration);
            break;
        case PH_DOWN:
            _PHCorrector.activePhDownPump(queuePumpData.duration);
            break;
        }

        _statusState = PUMP_ADDING;
        break;
    }

    case PUMP_ADDING:
    {
        bool pumpStatus = _PHCorrector.isActivePump() || _TDSCorrector.isActivePump();
        if (!pumpStatus)
        {
            _mainTank.activeMixer();
            _statusState = PUMP_SENDING;
        }
        break;
    }

    case PUMP_SENDING:
    {
        if (!_mainTank.isActiveMixer())
        {
            PumpData queuePumpData = _queuePump.front();

            if (queuePumpData.source == LOCALLY)
            {
                StaticJsonDocument<256> sendingPumpJson;
                sendingPumpJson["duration"] = queuePumpData.duration;
                sendingPumpJson["pump"] = _pumpTypeToString(queuePumpData.type);
                sendingPumpJson["datetime"] = queuePumpData.datetime;

                char buffer[256];
                serializeJson(sendingPumpJson, buffer);
                bool status = _MQTTManagement.publish("pump/locally", buffer);

                if (!status)
                {
                    _pendingPump.push_back(queuePumpData);
                }
            }

            _queuePump.pop();

            _mcp.digitalWrite(PC_LED_ADD_EVENT_PIN, LOW);

            _statusState = ENV_START;
        }
        break;
    }
    }
}

void DeviceIoT::_startReadEnvironment()
{
    if (_statusState == IDLE)
    {
        _statusState = ENV_START;
    }
}

void DeviceIoT::_startActivatePump(PumpType type, float duration, SourceType source)
{
    if (_statusState == IDLE || source != MANUALLY)
    {
        PumpData data;
        data.duration = duration;
        data.type = type;
        data.source = source;
        data.datetime = _networkManagement.getCurrentTime();

        _queuePump.push(data);

        _statusState = PUMP_START;
    }
}

void DeviceIoT::_sendTankNotificationHandle(String type, float volume)
{
    StaticJsonDocument<256> json;
    json["type"] = type;
    json["volume"] = volume;

    char buffer[256];
    serializeJson(json, buffer);
    _MQTTManagement.publish("tank/notification", buffer);
}

void DeviceIoT::_resendPumpEnvHandle()
{
    if (_pendingEnv.empty() && _pendingPump.empty())
        return;

    DynamicJsonDocument doc(4096);

    JsonArray envArray = doc.createNestedArray("environment");
    for (const EnvData &env : _pendingEnv)
    {
        JsonObject obj = envArray.createNestedObject();
        obj["ph"] = env.ph;
        obj["tds"] = env.tds;
        obj["temp"] = env.temp;
        obj["datetime"] = env.datetime;
    }

    JsonArray pumpArray = doc.createNestedArray("pump");
    for (const PumpData &pump : _pendingPump)
    {
        JsonObject obj = pumpArray.createNestedObject();
        obj["type"] = _pumpTypeToString(pump.type);
        obj["duration"] = pump.duration;
        obj["datetime"] = pump.datetime;
    }

    String buffer;
    serializeJson(doc, buffer);
    bool status = _MQTTManagement.publish("history/pending", buffer.c_str());

    if (status)
    {
        _pendingEnv.clear();
        _pendingPump.clear();
    }
}

String DeviceIoT::_pumpTypeToString(PumpType type)
{
    switch (type)
    {
    case NUTRITION:
        return "NUTRITION";
    case PH_UP:
        return "PH_UP";
    case PH_DOWN:
        return "PH_DOWN";
    default:
        return "UNKNOWN";
    }
}