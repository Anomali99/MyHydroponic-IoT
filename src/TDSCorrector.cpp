#include "TDSCorrector.h"
#include "Config.h"

TDSCorrector::TDSCorrector(Adafruit_MCP23X17 &mcp) : _mcp(mcp),
                                                     _levelASensor(UltrasonicSensor(TDS_A_US_TRIG_PIN, TDS_A_US_ECHO_PIN)),
                                                     _levelBSensor(UltrasonicSensor(TDS_B_US_TRIG_PIN, TDS_B_US_ECHO_PIN)),
                                                     _pinPump(TDS_TDS_PUMP_PIN)
{
}

void TDSCorrector::setup()
{
    _levelASensor.setup();
    _levelBSensor.setup();
    _mcp.pinMode(_pinPump, OUTPUT);
    _mcp.digitalWrite(_pinPump, HIGH);
}

void TDSCorrector::loop()
{
    static unsigned long lastLevelCheck = 0;
    unsigned long now = millis();

    if (_isActivePump)
    {
        if (now - _lastTimePump >= _pumpDuration)
        {
            _mcp.digitalWrite(_pinPump, HIGH);
            _isActivePump = false;
        }
    }
    else if (now - lastLevelCheck >= 500)
    {
        lastLevelCheck = now;

        if (_warningAStatus)
        {
            float ALevel = getALevelCm();
            if (ALevel > _ATankMinLevel)
            {
                _warningAStatus = false;
            }
        }

        if (_warningBStatus)
        {
            float BLevel = getBLevelCm();
            if (BLevel > _BTankMinLevel)
            {
                _warningBStatus = false;
            }
        }
    }
}

void TDSCorrector::activePump(float duration)
{
    if (!_isActivePump)
    {
        _isActivePump = true;
        _pumpDuration = duration * 1000;
        _lastTimePump = millis();
        _mcp.digitalWrite(_pinPump, LOW);
    }
}

float TDSCorrector::getALevelCm()
{
    float distance = _levelASensor.getDistanceCm();
    float level = _ATankHeight - distance;

    return level;
}

float TDSCorrector::getBLevelCm()
{
    float distance = _levelBSensor.getDistanceCm();
    float level = _BTankHeight - distance;

    return level;
}

float TDSCorrector::getACurrentVolume()
{
    float currenLevel = getALevelCm();

    if (currenLevel < _ATankMinLevel)
    {
        _warningAStatus = true;
    }

    float currentVolume = _ATankVolume * (currenLevel / _ATankMaxLevel);

    return currentVolume;
}

float TDSCorrector::getBCurrentVolume()
{
    float currenLevel = getBLevelCm();

    if (currenLevel < _BTankMinLevel)
    {
        _warningBStatus = true;
    }

    float currentVolume = _BTankVolume * (currenLevel / _BTankMaxLevel);

    return currentVolume;
}

SetupData TDSCorrector::getASetupData()
{
    SetupData data;
    data.name = "NutA";

    data.distance = _levelASensor.getSetupDistanceCm();
    data.level = _ATankHeight - data.distance;
    data.volume = _ATankVolume * (data.level / _ATankMaxLevel);

    return data;
}

SetupData TDSCorrector::getBSetupData()
{
    SetupData data;
    data.name = "NutB";

    data.distance = _levelBSensor.getSetupDistanceCm();
    data.level = _BTankHeight - data.distance;
    data.volume = _BTankVolume * (data.level / _BTankMaxLevel);

    return data;
}

bool TDSCorrector::isAWarning()
{
    return _warningAStatus;
}

bool TDSCorrector::isBWarning()
{
    return _warningBStatus;
}

bool TDSCorrector::isActivePump()
{
    return _isActivePump;
}