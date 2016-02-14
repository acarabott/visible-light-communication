#include "LED.h"
#include "LDR.h"

LED emitterLED(13);
LED outputLED(4);
LDR ldr(0);

uint32_t prevMicros = 0;
const uint32_t duration = 500000;

void setup()
{
}

void loop()
{
  uint32_t curMicros = micros();

  if(curMicros - prevMicros >= duration) {
    prevMicros = curMicros;
    emitterLED.toggle();
    ldr.update();
    outputLED.set(ldr.getBinary());
  }
}