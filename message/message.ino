#include "LED.h"

LED led(13);

void setup()
{
}

void loop()
{
  led.set(1);
  delay(500);
  led.set(0);
  delay(500);
}