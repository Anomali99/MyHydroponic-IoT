#include <Arduino.h>
#include "DeviceIoT.h"

DeviceIoT deviceIoT;

void setup()
{
  Serial.begin(115200);
  deviceIoT.setup();
}

void loop()
{
  deviceIoT.loop();
}