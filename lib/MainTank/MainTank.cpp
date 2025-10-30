#include "MainTank.h"

MainTank::MainTank(
    byte pinTrigLevel,
    byte pinEchoLevel,
    byte pinValve,
    byte pinMixer,
    float tankHeight,
    float minLevel,
    float maxLevel)
    : _levelSensor(UltrasonicSensor(pinTrigLevel, pinEchoLevel)),
      _tankHeight(tankHeight),
      _minLevel(minLevel),
      _maxLevel(maxLevel),
      _pinValve(pinValve),
      _pinMixer(pinMixer) {}

void MainTank::setup()
{
    _levelSensor.setup();
    pinMode(_pinValve, OUTPUT);
    pinMode(_pinMixer, OUTPUT);
}

void MainTank::activeMixer()
{
    digitalWrite(_pinMixer, HIGH);
    delay(3000);

    digitalWrite(_pinMixer, LOW);
}

float MainTank::getLevelCm()
{
    float distance = _levelSensor.getDistanceCm();
    float level = _tankHeight - distance;

    if (level < _minLevel)
    {
        digitalWrite(_pinValve, HIGH);
        while (level >= _maxLevel)
        {
            distance = _levelSensor.getDistanceCm();
            level = _tankHeight - distance;
        }
        digitalWrite(_pinValve, LOW);
    }

    return level;
}