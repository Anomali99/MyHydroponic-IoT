#include "PHCorrector.h"

PHCorrector::PHCorrector(
    byte pinTrigPhUpLevel,
    byte pinEchoPhUpLevel,
    byte pinTrigPhDownLevel,
    byte pinEchoPhDownLevel,
    byte pinPumpPhUp,
    byte pinPumpPhDown,
    float maxLevelPhUp,
    float maxLevelPhDown) : _levelPhUpSensor(UltrasonicSensor(pinTrigPhUpLevel, pinEchoPhUpLevel)),
                            _levelPhDownSensor(UltrasonicSensor(pinTrigPhDownLevel, pinEchoPhDownLevel)),
                            _maxLevelPhUp(pinPumpPhUp),
                            _maxLevelPhDown(pinPumpPhDown),
                            _pinPumpPhUp(maxLevelPhUp),
                            _pinPumpPhDown(maxLevelPhDown) {}

void PHCorrector::setup()
{
    _levelPhUpSensor.setup();
    _levelPhDownSensor.setup();
    pinMode(_pinPumpPhUp, OUTPUT);
    pinMode(_pinPumpPhDown, OUTPUT);
}

void PHCorrector::activePhUpPump(float duration)
{
    digitalWrite(_pinPumpPhUp, HIGH);
    delay(duration * 1000);
    digitalWrite(_pinPumpPhUp, LOW);
}

void PHCorrector::activePhDownPump(float duration)
{
    digitalWrite(_pinPumpPhDown, HIGH);
    delay(duration * 1000);
    digitalWrite(_pinPumpPhDown, LOW);
}

float PHCorrector::getPhUpLevelCm()
{
    float distance = _levelPhUpSensor.getDistanceCm();
    float level = _maxLevelPhUp - distance;

    return level;
}

float PHCorrector::getPhDownLevelCm()
{
    float distance = _levelPhDownSensor.getDistanceCm();
    float level = _maxLevelPhDown - distance;

    return level;
}