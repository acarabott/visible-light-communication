#define PATTERN_LENGTH 8
#define SLOTS 4
#define INTERVAL 500000
// #define DEBUG

struct LEDHardware {
  uint8_t pin;
  uint8_t state;
};

struct BlinkLED {
  LEDHardware hardware;
  uint32_t prevMicros;
  uint32_t interval;
};

struct PatternLED {
  LEDHardware hardware;
  uint32_t  prevMicros;
  uint32_t  interval;
  uint8_t   pattern[PATTERN_LENGTH];
  uint32_t  patternIdx;
  uint8_t   slots[SLOTS];
  uint32_t  slotIdx;
};

struct LDRHardware {
  uint8_t   pin;
  uint32_t  val;
  uint32_t  minVal;
  uint32_t  maxVal;
};

struct PatternLDR {
  LDRHardware hardware;
  uint32_t  prevMicros;
  uint32_t  interval;
  uint8_t   slots[SLOTS];
  uint32_t  slotIdx;
};

struct PatternLED led;
struct PatternLDR ldr;

const uint8_t outputPin = 4;

void setup() {
  #ifdef DEBUG
    Serial.begin(9600); // sets the serial port to 9600
  #endif

  led = {
    .hardware = { .pin = 13, .state = 0 },
    .prevMicros = 0,
    .interval = INTERVAL,
    .pattern = { 1, 1, 1, 1, 0, 0, 0, 0 },
    .patternIdx = 0,
    .slots = { 1, 1, 0, 0 },  // 0 = meaningless light, 1 = data
    .slotIdx = 0,
  };
  pinMode(led.hardware.pin, OUTPUT);

  ldr = {
    .hardware = { .pin = 0, .val = 0, .minVal = 1024, .maxVal = 0 },
    .prevMicros = 0,
    .interval = INTERVAL,
    .slots = { 1, 1, 0, 0 }, // 0 = ignore, 1 = read data
    .slotIdx = 0,
  };
  pinMode(ldr.hardware.pin, INPUT);

  pinMode(outputPin, OUTPUT);
}

void toggleLED(struct LEDHardware& led) {
  led.state = abs(1 - led.state);
  digitalWrite(led.pin, led.state);
}

void tryBlinkLED(struct BlinkLED& led, uint32_t currentMicros) {
  if(currentMicros - led.prevMicros >= led.interval) {
    led.prevMicros = currentMicros;
    toggleLED(led.hardware);
  }
}

void stepLED(struct PatternLED& led) {
  const uint8_t val = led.pattern[led.patternIdx];
  digitalWrite(led.hardware.pin, val);
  led.patternIdx = (led.patternIdx + 1) % PATTERN_LENGTH;

  #ifdef DEBUG
    Serial.print("step val: ");
    Serial.println(val);
  #endif
}

void tryPatternLED(struct PatternLED& led, uint32_t currentMicros) {
  if(currentMicros - led.prevMicros >= led.interval) {
    led.prevMicros = currentMicros;
    const uint8_t mode = led.slots[led.slotIdx];
    led.slotIdx = (led.slotIdx + 1) % SLOTS;

    if(mode == 1) {
      // output real data
      // stepLED(led);
      #ifdef DEBUG
        Serial.println("led data");
      #endif
    } else {
      // meaningless light slot
      // digitalWrite(led.hardware.pin, 1);
      #ifdef DEBUG
        Serial.println("led null");
      #endif
    }
  }
}

void updateLDR(struct LDRHardware& ldr) {
  ldr.val = analogRead(ldr.pin);
  // calibrate, and slowly squeeze
  ldr.minVal = min(ldr.minVal, ldr.val) + 1;
  ldr.maxVal = max(ldr.maxVal, ldr.val) - 1;

  // ensure that are min and max aren't too close
  const uint16_t minRange = 10;
  const uint8_t tooClose = ldr.minVal + minRange >= ldr.maxVal;

  ldr.minVal = tooClose ? ldr.minVal - (minRange / 2) : ldr.minVal;
  ldr.maxVal = tooClose ? ldr.maxVal + (minRange / 2) : ldr.maxVal;
}

uint8_t getLDRBinary(struct LDRHardware& ldr) {
  const uint16_t midpoint = (ldr.maxVal + ldr.minVal) / 2;
  return ldr.val >= midpoint;
}

void tryLDR(struct PatternLDR& ldr, uint32_t currentMicros) {
  if(currentMicros - ldr.prevMicros >= ldr.interval) {
    ldr.prevMicros = currentMicros;
    updateLDR(ldr.hardware);

    const uint8_t mode = ldr.slots[ldr.slotIdx];
    ldr.slotIdx = (ldr.slotIdx + 1) % SLOTS;
    if(mode == 1) {
      // read slot
      #ifdef DEBUG
        Serial.println("ldr data");
      #endif
    } else {
      // ignore slot

      #ifdef DEBUG
        Serial.println("ldr null");
      #endif
    }
    const int8_t ldrBin = getLDRBinary(ldr.hardware);

    // digitalWrite(outputPin, ldrBin);
    #ifdef DEBUG
      Serial.print("ldr val: ");
      Serial.print(ldr.hardware.val);
      Serial.print("\t");

      Serial.print("ldr min: ");
      Serial.print(ldr.hardware.minVal);
      Serial.print("\t");

      Serial.print("ldr max: ");
      Serial.print(ldr.hardware.maxVal);
      Serial.print("\t");

      Serial.print("ldr bin: ");
      Serial.print(ldrBin);
      Serial.print("\n");
    #endif
  }
}

void loop() {
  // Read and print input value every 10 microseconds
  const uint32_t currentMicros = micros();

  tryPatternLED(led, currentMicros);
  tryLDR(ldr, currentMicros);
  // tryBlinkLED(led, currentMicros);
}