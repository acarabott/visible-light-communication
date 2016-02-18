#include "LED.h"
#include "LDR.h"
#include "Photodiode.h"
#include <LiquidCrystal.h>

// #define LOG
// #define PRINT

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

LED emitterLED(13);
Photodiode photodiode(0);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

uint32_t prevMicros = 0;
const uint32_t duration = 1000;

#define ENCODING_LENGTH 2
#define BYTE 8
#define PATTERN_LENGTH 8
#define ETX 3

uint8_t text[] = "Slow down!";
uint16_t textIdx = 0;
uint8_t pattern[BYTE] = { 0,1,0,0,0,0,0,1 }; // ASCII A
uint32_t patternIdx = 0;
uint32_t patternCount = 0;
uint32_t clockIdx = 0;

#define BUFFER_SIZE BYTE
uint8_t receiverBuffer[BUFFER_SIZE] = {0};
uint8_t receiverBufferIdx = 0;

#define LCD_COLS 16
#define LCD_ROWS 2
#define MAX_MSG_SIZE LCD_COLS
char msgBuffer[MAX_MSG_SIZE] = {0};
uint16_t msgBufferIdx = 0;

void setup()
{
  // can terminate strings with ETX
  // text[COUNT_OF(text) - 1] = ETX;
  // set up the LCD's number of columns and rows:
  lcd.begin(LCD_COLS, LCD_ROWS);
  #ifdef PRINT
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
const uint8_t getCharFromBuffer(uint8_t(&buffer)[BYTE]) {
  uint8_t myChar = 0;
  for(uint8_t i = 0; i < BYTE; i++ ) {
    if (buffer[(BYTE-1) - i] == 1) {
      myChar |= 1 << i;
    }
  }
  return myChar;
}

// note max size is uint16_t
void printCharFromBuffer(uint8_t(&buffer)[BUFFER_SIZE]) {
  Serial.write(getCharFromBuffer(buffer));
}

void encodeChar(uint8_t c, uint8_t(&buffer)[BYTE]) {
  for (uint16_t i = 0; i < BYTE; ++i) {
    const uint8_t val = (c >> i) & 1;
    buffer[(BYTE-1) - i] = val;
  }
}

void loop()
{
  uint32_t curMicros = micros();

  if(curMicros - prevMicros >= duration) {
    prevMicros = curMicros;

    if(clockIdx % ENCODING_LENGTH == 0 && patternIdx == 0) {
      encodeChar(text[textIdx], pattern);
      // message may be truncated
      textIdx = (textIdx + 1) % MAX_MSG_SIZE;
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
    // IMPORTANT!
    // if using ldr, then use clockIdx frame 1 not 0 because the ldr is always
    // behind by one frame!
    // photodiode is faster so use clockIdx % ENCODING_LENGTH == 0
    if(clockIdx % ENCODING_LENGTH == 0) {
      photodiode.update();

      #ifdef LOG
        Serial.print("ldr min: "); Serial.println(photodiode.getMinVal());
        Serial.print("ldr max: "); Serial.println(photodiode.getMaxVal());
        Serial.print("ldr val: "); Serial.println(photodiode.getVal());
        Serial.println("---");
      #endif

      // only add to buffer if
      const bool alreadyEnteredBuffer = receiverBufferIdx != 0;
      const bool firstItemInPatternBuffer = clockIdx % BUFFER_SIZE == 0;

      if(alreadyEnteredBuffer || firstItemInPatternBuffer) {
        // read value into buffer
        // const uint8_t receiverValue = ldr.getBinary();
        const uint8_t receiverValue = photodiode.getBinary();
        receiverBuffer[receiverBufferIdx] = receiverValue;

        // char complete
        if(receiverBufferIdx == (BYTE - 1)) {
          uint8_t receivedChar = getCharFromBuffer(receiverBuffer);
          msgBuffer[msgBufferIdx] = receivedChar;
          // end of message
          const bool bufferFull = msgBufferIdx == MAX_MSG_SIZE - 1;
          const bool endOfText = receivedChar == ETX;
          if(bufferFull || endOfText) {
            const String str(msgBuffer);
            lcd.setCursor(0, 0);
            lcd.print(msgBuffer);
            msgBufferIdx = 0;
          } else {
            msgBufferIdx = (msgBufferIdx + 1) % MAX_MSG_SIZE;
          }
        }

        // advance buffer index
        receiverBufferIdx = (receiverBufferIdx + 1) % BYTE;
      }
    }
    clockIdx++;
  }
}