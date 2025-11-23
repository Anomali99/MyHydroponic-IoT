#include "TDSCorrector.h"
#include "Config.h"

TDSCorrector::TDSCorrector(Adafruit_MCP23X17 mcp) : _mcp(mcp),
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
}

void TDSCorrector::loop()
{
    static unsigned long lastLevelCheck = 0;
    unsigned long now = millis();

    if (now - lastLevelCheck >= 5000)
    {
        lastLevelCheck = now;
        float ALevel = getALevelCm();
        float BLevel = getBLevelCm();
        if (ALevel > _ATankMinLevel && BLevel > _BTankMinLevel)
            _warningStatus = false;
        else
            _warningStatus = true;
    }
}

void TDSCorrector::activePump(float duration)
{
    _mcp.digitalWrite(_pinPump, HIGH);
    delay(duration * 1000);
    _mcp.digitalWrite(_pinPump, LOW);
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
    float currentVolume = _ATankVolume * (currenLevel / _ATankMaxLevel);

    return currentVolume;
}

float TDSCorrector::getBCurrentVolume()
{
    float currenLevel = getBLevelCm();
    float currentVolume = _BTankVolume * (currenLevel / _BTankMaxLevel);

    return currentVolume;
}

bool TDSCorrector::isWarning()
{
    return _warningStatus;
}