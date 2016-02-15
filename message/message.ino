#include "LED.h"
#include "LDR.h"

#define LOG

LED emitterLED(13);
LED outputLED(4);
LDR ldr(0);

uint32_t prevMicros = 0;
const uint32_t duration = 2000;

#define ENCODING_LENGTH 2
#define BITS 8
// send this at the start of the message for synchronisation
const uint8_t startSignal[BITS] = { 1, 1, 1, 1, 1, 1, 1, 1 };

#define PATTERN_LENGTH 8
uint8_t pattern[PATTERN_LENGTH] = { 1, 0, 1, 1, 0, 1, 0, 0 };
uint32_t patternIdx = 0;
uint32_t patternCount = 0;
uint32_t clockIdx = 0;

#define BUFFER_SIZE BITS
uint8_t receiverBuffer[BUFFER_SIZE];
uint8_t receiverBufferIdx = 0;

void setup()
{
  #ifdef LOG
    Serial.begin(115200);
  #endif
}

const uint8_t getManchesterEncoding(uint8_t signal, uint32_t clockIdx) {
  // get encoding value depending on value and clock position
  // e.g. 0 will be encoded in two values as:
  // clock@0 : 0, clock@1 : 1
  // 1 encoded as
  // clock@0 : 1, clock@1 : 0
  //
  // IMPORTANT! using frame clockIdx frame 1 not 0 because the ldr is always
  // behind by one frame!
  return signal == 0 ? (clockIdx % ENCODING_LENGTH == 1 ? 0 : 1) :
                       (clockIdx % ENCODING_LENGTH == 1 ? 1 : 0);
}

// note max size is uint16_t
void printBuffer(uint8_t(&buffer)[BUFFER_SIZE]) {
  for(uint16_t i=0; i < BUFFER_SIZE; i++) {
    Serial.print(buffer[i]);
  }
  Serial.print("\n");
}

void loop()
{
  uint32_t curMicros = micros();

  if(curMicros - prevMicros >= duration) {
    prevMicros = curMicros;

    const uint8_t patternVal = pattern[patternIdx];
    // increase pattern index every 2 steps of the clock as each value is
    // encoded as a pair of values
    if(clockIdx % ENCODING_LENGTH == 1){
      patternIdx = (patternIdx + 1) % PATTERN_LENGTH;
    }

    const uint8_t output = getManchesterEncoding(patternVal, clockIdx);

    emitterLED.set(output);
    ldr.update();
    // only update output every other frame, we
    if(clockIdx % ENCODING_LENGTH == 0) {
      // read value into buffer
      const uint8_t receiverValue = ldr.getBinary();
      receiverBuffer[receiverBufferIdx] = receiverValue;

      // message complete
      if(receiverBufferIdx == (BUFFER_SIZE - 1)) {
        printBuffer(receiverBuffer);
      }

      // advance buffer index
      receiverBufferIdx = (receiverBufferIdx + 1) % BUFFER_SIZE;

      // Serial.println(receiverValue);
      // outputLED.set(receiverValue);
    }

    clockIdx++;
  }
}