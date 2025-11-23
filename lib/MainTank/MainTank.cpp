#include "MainTank.h"

MainTank::MainTank(Adafruit_MCP23X17 mcp)
    : NO_CHANGE_TIMEOUT(10000),
      LEVEL_INCREASE_THRESHOLD(0.5),
      _mcp(mcp),
      _levelSensor(UltrasonicSensor(MT_US_TRIG_PIN, MT_US_ECHO_PIN)),
      _warningStatus(false),
      _tankVolume(tankVolume),
      _tankHeight(tankHeight),
      _minLevel(minLevel),
      _maxLevel(maxLevel),
      _pinValve(MT_VALVE_PIN),
      _pinMixer(MT_MIXER_PIN) {}

void MainTank::setup()
{
    _levelSensor.setup();
    _mcp.pinMode(_pinValve, OUTPUT);
    _mcp.pinMode(_pinMixer, OUTPUT);
}

void MainTank::loop()
{
    static unsigned long lastLevelCheck = 0;
    unsigned long now = millis();

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
    _mcp.digitalWrite(_pinMixer, HIGH);
    delay(3000);

    _mcp.digitalWrite(_pinMixer, LOW);
}

float MainTank::getLevelCm()
{
    float distance = _levelSensor.getDistanceCm();
    float currentLevel = _tankHeight - distance;

    if (currentLevel <= _minLevel)
    {
        float levelAtLastCheck = currentLevel;
        unsigned long lastChangeTime = millis();
        bool timeoutOccurred = false;

        _mcp.digitalWrite(_pinValve, HIGH);

        while (currentLevel <= _maxLevel)
        {
            unsigned long now = millis();

            distance = _levelSensor.getDistanceCm();
            currentLevel = _tankHeight - distance;

            if (currentLevel > levelAtLastCheck + LEVEL_INCREASE_THRESHOLD)
            {
                levelAtLastCheck = currentLevel;
                lastChangeTime = now;
            }

            if (now - lastChangeTime > NO_CHANGE_TIMEOUT)
            {
                timeoutOccurred = true;
                break;
            }

            delay(100);
        }

        _mcp.digitalWrite(_pinValve, LOW);
    }

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