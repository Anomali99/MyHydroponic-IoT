#include "PHCorrector.h"
#include "Config.h"

PHCorrector::PHCorrector(Adafruit_MCP23X17 mcp) : _mcp(mcp),
                                                  _levelPhUpSensor(UltrasonicSensor(PH_UP_US_TRIG_PIN, PH_UP_US_ECHO_PIN)),
                                                  _levelPhDownSensor(UltrasonicSensor(PH_DOWN_US_TRIG_PIN, PH_DOWN_US_ECHO_PIN)),
                                                  _pinPumpPhUp(PH_UP_PUMP_PIN),
                                                  _pinPumpPhDown(PH_DOWN_PUMP_PIN),
                                                  _warningStatus(false),
                                                  _phUpTankVolume(1.0),
                                                  _phDownTankVolume(1.0),
                                                  _phUpTankMaxLevel(40.0),
                                                  _phDownTankMaxLevel(40.0),
                                                  _phUpTankMinLevel(10.0),
                                                  _phDownTankMinLevel(10.0),
                                                  _phUpTankHeight(60.0),
                                                  _phDownTankHeight(60.0) {}

void PHCorrector::setup()
{
    _levelPhUpSensor.setup();
    _levelPhDownSensor.setup();
    _mcp.pinMode(_pinPumpPhUp, OUTPUT);
    _mcp.pinMode(_pinPumpPhDown, OUTPUT);
}

void PHCorrector::loop()
{
    static unsigned long lastLevelCheck = 0;
    unsigned long now = millis();

    if (now - lastLevelCheck >= 5000)
    {
        lastLevelCheck = now;
        float phUpLevel = getPhUpLevelCm();
        float phDownLevel = getPhDownLevelCm();
        if (phUpLevel > _phUpTankMinLevel && phDownLevel > _phDownTankMinLevel)
            _warningStatus = false;
        else
            _warningStatus = true;
    }
}

void PHCorrector::activePhUpPump(float duration)
{
    _mcp.digitalWrite(_pinPumpPhUp, HIGH);
    delay(duration * 1000);
    _mcp.digitalWrite(_pinPumpPhUp, LOW);
}

void PHCorrector::activePhDownPump(float duration)
{
    _mcp.digitalWrite(_pinPumpPhDown, HIGH);
    delay(duration * 1000);
    _mcp.digitalWrite(_pinPumpPhDown, LOW);
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
    float currentVolume = _phUpTankVolume * (currenLevel / _phUpTankMaxLevel);

    return currentVolume;
}

float PHCorrector::getPhDownCurrentVolume()
{
    float currenLevel = getPhDownLevelCm();
    float currentVolume = _phDownTankVolume * (currenLevel / _phDownTankMaxLevel);

    return currentVolume;
}

bool PHCorrector::isWarning()
{
    return _warningStatus;
}