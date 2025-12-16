#include "UltrasonicSensor.h"

UltrasonicSensor::UltrasonicSensor(byte pinTrig, byte pinEcho)
    : _pinTrig(pinTrig), _pinEcho(pinEcho)
{
}

void UltrasonicSensor::setup()
{
    pinMode(_pinTrig, OUTPUT);
    pinMode(_pinEcho, INPUT);
}

float UltrasonicSensor::getDistanceCm()
{
    for (int i = 0; i < _maxRetries; i++)
    {
        digitalWrite(_pinTrig, LOW);
        delayMicroseconds(5);

        digitalWrite(_pinTrig, HIGH);
        delayMicroseconds(10);
        digitalWrite(_pinTrig, LOW);

        long duration = pulseIn(_pinEcho, HIGH);
        float distance = duration * 0.0344 / 2;

        if (distance > 22.0)
        {
            return distance;
        }

        delay(15);
    }

    return -1.0;
}

float UltrasonicSensor::getSetupDistanceCm()
{
    digitalWrite(_pinTrig, LOW);
    delayMicroseconds(5);

    digitalWrite(_pinTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(_pinTrig, LOW);

    long duration = pulseIn(_pinEcho, HIGH);
    return duration * 0.0344 / 2;
}
