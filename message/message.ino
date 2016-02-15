#include "LED.h"
#include "LDR.h"

// #define LOG

LED emitterLED(13);
LED outputLED(4);
LDR ldr(0);

uint32_t prevMicros = 0;
const uint32_t duration = 500000;
// const uint32_t duration = 500;

#define PATTERN_LENGTH 4
uint8_t pattern[PATTERN_LENGTH] = { 1, 1, 1, 0 };
uint32_t patternIdx = 0;
uint32_t patternCount = 0;
uint32_t clockIdx = 0;


void setup()
{
  #ifdef LOG
    Serial.begin(9600);
  #endif
}

void loop()
{
  uint32_t curMicros = micros();

  if(curMicros - prevMicros >= duration) {
    prevMicros = curMicros;

    const uint8_t patternVal = pattern[patternIdx];
    // increase pattern index every 2 steps of the clock as each value is
    // encoded as a pair of values: 0 = [0, 1], 1 = [1, 0]
    if(clockIdx % 2 == 1){
      patternIdx = (patternIdx + 1) % PATTERN_LENGTH;
    }

    // if the value is 0 it will be encoded over two steps, depending on our
    // clock index
    const uint8_t output = patternVal == 0 ? (clockIdx % 2 == 0 ? 0 : 1) :
                                             (clockIdx % 2 == 0 ? 1 : 0);

    #ifdef LOG
      Serial.print(clockIdx % 8);
      Serial.print(": ");
      Serial.print(patternVal);
      Serial.print(" - ");
      Serial.println(output);
    #endif

    emitterLED.set(output);
    ldr.update();
    outputLED.set(ldr.getBinary());

    clockIdx++;
  }
}