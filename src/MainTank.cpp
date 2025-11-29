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

    if (_isAddingWater)
    {
        float currentLevel = getLevelCm();

        if (currentLevel > _levelAtLastCheck + LEVEL_INCREASE_THRESHOLD)
        {
            _lastTimeCheck = now;
            _levelAtLastCheck = currentLevel;
            _warningStatus = false;
        }

        if (now - _lastTimeCheck > NO_CHANGE_TIMEOUT)
        {
            _warningStatus = true;
        }

        if (currentLevel >= _maxLevel)
        {
            _mcp.digitalWrite(_pinValve, HIGH);
            _warningStatus = false;
            _isAddingWater = false;
        }
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

    return currentLevel;
}

float MainTank::getCurrentVolume()
{
    float currentLevel = getLevelCm();

    if (currentLevel < _minLevel && !_isAddingWater)
    {
        _isAddingWater = true;
        _mcp.digitalWrite(_pinValve, LOW);

        _lastTimeCheck = millis();
        _levelAtLastCheck = currentLevel;
        _warningStatus = false;
    }

    float currentVolume = _tankVolume * (currentLevel / _maxLevel);

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