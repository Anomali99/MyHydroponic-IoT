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
    static unsigned long lastLevelCheck = 0;
    unsigned long now = millis();

    if (_isActivePump)
    {
        if (now - _lastTimePump >= _pumpDuration)
        {
            _mcp.digitalWrite(_pinPumpPhUp, HIGH);
            _mcp.digitalWrite(_pinPumpPhDown, HIGH);
            _isActivePump = false;
        }
    }
    else if (now - lastLevelCheck >= 500)
    {
        lastLevelCheck = now;

        if (_warningUpStatus)
        {
            float phUpLevel = getPhUpLevelCm();
            if (phUpLevel > _phUpTankMinLevel)
            {
                _warningUpStatus = false;
            }
        }

        if (_warningDownStatus)
        {
            float phDownLevel = getPhDownLevelCm();
            if (phDownLevel > _phDownTankMinLevel)
            {
                _warningDownStatus = false;
            }
        }
    }
}

void PHCorrector::activePhUpPump(float duration)
{
    if (!_isActivePump)
    {
        _isActivePump = true;
        _pumpDuration = duration * 1000;
        _lastTimePump = millis();
        _mcp.digitalWrite(_pinPumpPhUp, LOW);
    }
}

void PHCorrector::activePhDownPump(float duration)
{
    if (!_isActivePump)
    {
        _isActivePump = true;
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
    float level = _pinPumpPhDown - distance;

    return level;
}

float PHCorrector::getPhUpCurrentVolume()
{
    float currenLevel = getPhUpLevelCm();

    if (currenLevel < _phUpTankMinLevel)
    {
        _warningUpStatus = true;
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

    float currentVolume = _phDownTankVolume * (currenLevel / _phDownTankMaxLevel);

    return currentVolume;
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
    return _isActivePump;
}