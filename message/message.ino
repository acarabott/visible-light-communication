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

const uint8_t getManchesterEncoding(uint8_t signal, uint32_t clockIdx) {
  // get encoding value depending on value and clock position
  // e.g. 0 will be encoded in two values as:
  // clock@0 : 0, clock@1 : 1
  // 1 encoded as
  // clock@0 : 1, clock@1 : 0
  return signal == 0 ? (clockIdx % 2 == 0 ? 0 : 1) :
                       (clockIdx % 2 == 0 ? 1 : 0);
}

void loop()
{
  uint32_t curMicros = micros();

  if(curMicros - prevMicros >= duration) {
    prevMicros = curMicros;

    const uint8_t patternVal = pattern[patternIdx];
    // increase pattern index every 2 steps of the clock as each value is
    // encoded as a pair of values
    if(clockIdx % 2 == 1){
      patternIdx = (patternIdx + 1) % PATTERN_LENGTH;
    }

    const uint8_t output = getManchesterEncoding(patternVal, clockIdx);

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