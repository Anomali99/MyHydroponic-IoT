#include <Arduino.h>
#include "PhysicalControl.h"

PhysicalControl physicalControl;

void setup()
{
  Serial.begin(115200);
  physicalControl.setup();
}

void loop()
{
  physicalControl.loop();
}