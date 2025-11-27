#include "MainTank.h"
#include "Config.h"

MainTank::MainTank(Adafruit_MCP23X17 &mcp)
    : _mcp(mcp),
      _levelSensor(UltrasonicSensor(MT_US_TRIG_PIN, MT_US_ECHO_PIN)),
      _pinValve(MT_VALVE_PIN),
      _pinMixer(MT_MIXER_PIN) {}

void MainTank::setup()
{
    _levelSensor.setup();
    _mcp.pinMode(_pinValve, OUTPUT);
    _mcp.pinMode(_pinMixer, OUTPUT);
    _mcp.digitalWrite(_pinValve, HIGH);
    _mcp.digitalWrite(_pinMixer, HIGH);
}

void MainTank::loop()
{
    static unsigned long lastLevelCheck = 0;
    unsigned long now = millis();

    if (_isActiveMixer)
    {
        if (now - _lastTimeMixer >= _mixerDuration)
        {
            _mcp.digitalWrite(_pinMixer, HIGH);
            _isActiveMixer = false;
        }
    }

    if (now - lastLevelCheck >= 5000)
    {
        lastLevelCheck = now;
        float distance = _levelSensor.getDistanceCm();
        float currentLevel = _tankHeight - distance;
        if (currentLevel > _minLevel)
            _warningStatus = false;
        else
            _warningStatus = true;
    }
}

void MainTank::activeMixer()
{
    if (!_isActiveMixer)
    {
        _mcp.digitalWrite(_pinMixer, LOW);
        _lastTimeMixer = millis();
        _isActiveMixer = true;
    }
}

float MainTank::getLevelCm()
{
    float distance = _levelSensor.getDistanceCm();
    float currentLevel = _tankHeight - distance;

    // if (currentLevel <= _minLevel)
    // {
    //     float levelAtLastCheck = currentLevel;
    //     unsigned long lastChangeTime = millis();
    //     bool timeoutOccurred = false;

    //     _mcp.digitalWrite(_pinValve, LOW);

    //     while (currentLevel <= _maxLevel)
    //     {
    //         unsigned long now = millis();

    //         distance = _levelSensor.getDistanceCm();
    //         currentLevel = _tankHeight - distance;

    //         if (currentLevel > levelAtLastCheck + LEVEL_INCREASE_THRESHOLD)
    //         {
    //             levelAtLastCheck = currentLevel;
    //             lastChangeTime = now;
    //         }

    //         if (now - lastChangeTime > NO_CHANGE_TIMEOUT)
    //         {
    //             timeoutOccurred = true;
    //             break;
    //         }

    //         delay(100);
    //     }

    //     _mcp.digitalWrite(_pinValve, HIGH);
    // }

    return currentLevel;
}

float MainTank::getCurrentVolume()
{
    float currenLevel = getLevelCm();
    float currentVolume = _tankVolume * (currenLevel / _maxLevel);

    return currentVolume;
}

bool MainTank::isWarning()
{
    return _warningStatus;
}

bool MainTank::isActiveMixer()
{
    return _isActiveMixer;
}