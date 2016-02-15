#include "LED.h"
#include "LDR.h"

#define LOG

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))


LED emitterLED(13);
LED outputLED(4);
LDR ldr(0);

uint32_t prevMicros = 0;
const uint32_t duration = 1000;

#define ENCODING_LENGTH 2
#define BYTE 8
#define PATTERN_LENGTH 8
uint8_t text[] = "Hello World!\n";
uint16_t textIdx = 0;
uint8_t pattern[BYTE] = { 0,1,0,0,0,0,0,1 }; // ASCII A
uint32_t patternIdx = 0;
uint32_t patternCount = 0;
uint32_t clockIdx = 0;

#define BUFFER_SIZE BYTE
uint8_t receiverBuffer[BUFFER_SIZE] = {0};
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
  return signal == 0 ? (clockIdx % ENCODING_LENGTH == 0 ? 0 : 1) :
                       (clockIdx % ENCODING_LENGTH == 0 ? 1 : 0);
}

// note max size is uint16_t
void printBuffer(uint8_t(&buffer)[BUFFER_SIZE]) {
  uint8_t myByte = 0;
  for(uint8_t i = 0; i < BUFFER_SIZE; i++ ) {
    if (buffer[(BUFFER_SIZE-1) - i] == 1) {
      myByte |= 1 << i;
    }
  }
  Serial.write(myByte);
}

void encodeChar(uint8_t c, uint8_t(&buffer)[BUFFER_SIZE]) {
  for (uint16_t i = 0; i < BUFFER_SIZE; ++i) {
    const uint8_t val = (c >> i) & 1;
    buffer[(BUFFER_SIZE-1) - i] = val;
  }
}

void loop()
{
  uint32_t curMicros = micros();

  if(curMicros - prevMicros >= duration) {
    prevMicros = curMicros;

    if(clockIdx % ENCODING_LENGTH == 0 && patternIdx == 0) {
      encodeChar(text[textIdx], pattern);
      textIdx = (textIdx + 1) % COUNT_OF(text);
    }

    const uint8_t patternVal = pattern[patternIdx];
    // increase pattern index every 2 steps of the clock as each value is
    // encoded as a pair of values
    if(clockIdx % ENCODING_LENGTH == 1){
      patternIdx = (patternIdx + 1) % PATTERN_LENGTH;
    }

    const uint8_t output = getManchesterEncoding(patternVal, clockIdx);

    emitterLED.set(output);
    // only update output every other frame, because of 2 bit encoding
    // IMPORTANT! using frame clockIdx frame 1 not 0 because the ldr is always
    // behind by one frame!
    // Otherwise this firs check would be clockIdx % ENCODING_LENGTH == 0
    if(clockIdx % ENCODING_LENGTH == 1) {
      ldr.update();
      // only add to buffer if
      const bool alreadyEnteredBuffer = receiverBufferIdx != 0;
      const bool firstItemInPatternBuffer = clockIdx % BUFFER_SIZE == 1;

      if(alreadyEnteredBuffer || firstItemInPatternBuffer) {
        // read value into buffer
        const uint8_t receiverValue = ldr.getBinary();
        receiverBuffer[receiverBufferIdx] = receiverValue;

        // message complete
        if(receiverBufferIdx == (BUFFER_SIZE - 1)) {
          printBuffer(receiverBuffer);
        }

        // advance buffer index
        receiverBufferIdx = (receiverBufferIdx + 1) % BUFFER_SIZE;
      }
    }
    clockIdx++;
  }
}