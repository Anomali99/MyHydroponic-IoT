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
    digitalWrite(_pinTrig, LOW);
    delayMicroseconds(5);

    digitalWrite(_pinTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(_pinTrig, LOW);

    long duration = pulseIn(_pinEcho, HIGH);
    float distance = duration * 0.034 / 2;

    return distance;
}
