#include "LED.h"
#include "LDR.h"

LED emitterLED(13);
LED outputLED(4);
LDR ldr(0);

uint32_t prevMicros = 0;
const uint32_t duration = 100000;

const uint8_t steps = 2;
uint32_t idx = 0;
uint8_t emitterState = 0;

void setup()
{
  // Serial.begin(9600);
}

void loop()
{
  uint32_t curMicros = micros();

  if(curMicros - prevMicros >= duration) {
    prevMicros = curMicros;

    if(idx % steps == 0) {
      // Serial.println("EMIT ON IGNORE!");
      emitterLED.set(1);
      ldr.update();
      outputLED.set(ldr.getBinary());
    } else {
      // Serial.print("EMIT state: ");
      // Serial.println(emitterState);
      emitterLED.set(emitterState);
      emitterState = abs(1 - emitterState);

      // Serial.print("LDR READING: ");
      // Serial.println(ldr.getBinary());

    }
    // Serial.println("---");
    idx++;
  }
}