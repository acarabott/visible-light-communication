#include "LED.h"
#include "LDR.h"

LED emitterLED(13);
LED outputLED(4);
LDR ldr(0);

uint32_t prevMicros = 0;
const uint32_t duration = 500;

const uint8_t steps = 2;
uint32_t idx = 0;
uint8_t emitterState = 0;

#define PATTERN_LENGTH 4
uint8_t pattern[PATTERN_LENGTH] = { 1, 1, 1, 0 };
uint32_t patternIdx = 0;
uint32_t patternCount = 0;

void setup()
{
}

void loop()
{
  uint32_t curMicros = micros();

  if(curMicros - prevMicros >= duration) {
    prevMicros = curMicros;

    if(idx % steps == 0) {
      emitterLED.set(1);
      ldr.update();
      outputLED.set(ldr.getBinary());
    } else {
      emitterLED.set(emitterState);
      emitterState = pattern[patternIdx % PATTERN_LENGTH];

      if(patternCount % 512 == 0) {
        patternIdx++;
      }
      patternCount++;
    }
    idx++;
  }
}