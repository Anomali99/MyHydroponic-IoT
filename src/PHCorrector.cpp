#include "PHCorrector.h"
#include "Config.h"

PHCorrector::PHCorrector(Adafruit_MCP23X17 &mcp) : _mcp(mcp),
                                                   _levelPhUpSensor(UltrasonicSensor(PH_UP_US_TRIG_PIN, PH_UP_US_ECHO_PIN)),
                                                   _levelPhDownSensor(UltrasonicSensor(PH_DOWN_US_TRIG_PIN, PH_DOWN_US_ECHO_PIN)),
                                                   _pinPumpPhUp(PH_UP_PUMP_PIN),
                                                   _pinPumpPhDown(PH_DOWN_PUMP_PIN) {}

void PHCorrector::setup()
{
    _levelPhUpSensor.setup();
    _levelPhDownSensor.setup();
    _mcp.pinMode(_pinPumpPhUp, OUTPUT);
    _mcp.pinMode(_pinPumpPhDown, OUTPUT);
    _mcp.digitalWrite(_pinPumpPhUp, HIGH);
    _mcp.digitalWrite(_pinPumpPhDown, HIGH);
}

void PHCorrector::loop()
{
    static unsigned long lastKeepAlive = 0;
    unsigned long now = millis();

    if (_isActiveUpPump || _isActiveDownPump)
    {

        if (now - lastKeepAlive > 100)
        {
            lastKeepAlive = now;

            if (_isActiveUpPump)
            {
                _mcp.digitalWrite(_pinPumpPhUp, LOW);
            }
            if (_isActiveDownPump)
            {
                _mcp.digitalWrite(_pinPumpPhDown, LOW);
            }
        }

        if (now - _lastTimePump >= _pumpDuration)
        {
            if (_isActiveUpPump)
            {
                _isActiveUpPump = false;
                _mcp.digitalWrite(_pinPumpPhUp, HIGH);
            }
            if (_isActiveDownPump)
            {
                _isActiveDownPump = false;
                _mcp.digitalWrite(_pinPumpPhDown, HIGH);
            }
        }
    }
}

void PHCorrector::activePhUpPump(float duration)
{
    if (!_isActiveUpPump && !_isActiveDownPump)
    {
        _isActiveUpPump = true;
        _pumpDuration = duration * 1000;
        _lastTimePump = millis();
        _mcp.digitalWrite(_pinPumpPhUp, LOW);
    }
}

void PHCorrector::activePhDownPump(float duration)
{
    if (!_isActiveUpPump && !_isActiveDownPump)
    {
        _isActiveDownPump = true;
        _pumpDuration = duration * 1000;
        _lastTimePump = millis();
        _mcp.digitalWrite(_pinPumpPhDown, LOW);
    }
}

float PHCorrector::getPhUpLevelCm()
{
    float distance = _levelPhUpSensor.getDistanceCm();
    float level = _phUpTankHeight - distance;

    return level;
}

float PHCorrector::getPhDownLevelCm()
{
    float distance = _levelPhDownSensor.getDistanceCm();
    float level = _phDownTankHeight - distance;

    return level;
}

float PHCorrector::getPhUpCurrentVolume()
{
    float currenLevel = getPhUpLevelCm();

    if (currenLevel < _phUpTankMinLevel)
    {
        _warningUpStatus = true;
    }
    else
    {
        _warningUpStatus = false;
    }

    float currentVolume = _phUpTankVolume * (currenLevel / _phUpTankMaxLevel);

    return currentVolume;
}

float PHCorrector::getPhDownCurrentVolume()
{
    float currenLevel = getPhDownLevelCm();

    if (currenLevel < _phDownTankMinLevel)
    {
        _warningDownStatus = true;
    }
    else
    {
        _warningDownStatus = false;
    }

    float currentVolume = _phDownTankVolume * (currenLevel / _phDownTankMaxLevel);

    return currentVolume;
}

SetupData PHCorrector::getPhUpSetupData()
{
    SetupData data;
    data.name = "PhUp";

    data.distance = _levelPhUpSensor.getSetupDistanceCm();
    data.level = _phUpTankHeight - data.distance;
    data.volume = _phUpTankVolume * (data.level / _phUpTankMaxLevel);

    return data;
}

SetupData PHCorrector::getPhDownSetupData()
{
    SetupData data;
    data.name = "PhDn";

    data.distance = _levelPhDownSensor.getSetupDistanceCm();
    data.level = _phDownTankHeight - data.distance;
    data.volume = _phDownTankVolume * (data.level / _phDownTankMaxLevel);

    return data;
}

bool PHCorrector::isUpWarning()
{
    return _warningUpStatus;
}

bool PHCorrector::isDownWarning()
{
    return _warningDownStatus;
}

bool PHCorrector::isActivePump()
{
    return _isActiveUpPump || _isActiveDownPump;
}