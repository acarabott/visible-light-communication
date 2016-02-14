struct LEDHardware {
  uint8_t pin;
  uint8_t state;
};

struct BlinkLED {
  LEDHardware hardware;
  uint32_t prevMicros;
  uint32_t interval;
};

#define PATTERN_LENGTH 8
#define SLOTS 4
struct PatternLED {
  LEDHardware hardware;
  uint32_t  prevMicros;
  uint32_t  interval;
  uint8_t   patternLength;
  uint8_t   pattern[PATTERN_LENGTH];
  uint32_t  patternIdx;
  uint32_t  slots[SLOTS];
  uint32_t  slotIdx;
};

struct LDR {
  uint8_t   pin;
  uint32_t  prevMicros;
  uint32_t  interval;
  uint32_t  val;
  uint32_t  minVal;
  uint32_t  maxVal;
};

const uint8_t printEnabled = 1;

struct PatternLED led;
struct LDR ldr;

const uint8_t outputPin = 4;

void setup() {
  if(printEnabled) {
    Serial.begin(9600); // sets the serial port to 9600
  }

  led = {
    .hardware = { .pin = 13, .state = 0 },
    .prevMicros = 0,
    .interval = 500000,
    .patternLength = PATTERN_LENGTH,
    .pattern = { 1, 1, 1, 1, 0, 0, 0, 0 },
    .patternIdx = 0,
    .slots = { 1, 1, 0, 0 },
    .slotIdx = 0,
  };
  pinMode(led.hardware.pin, OUTPUT);

  ldr = {
    .pin = 0,
    .prevMicros = 0,
    .interval = 500,
    .val = 0,
    .minVal = 9999,
    .maxVal = 0,
  };
  pinMode(ldr.pin, INPUT);

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
  const uint32_t val = led.pattern[led.patternIdx];
  Serial.print("step val: ");
  Serial.println(val);
  digitalWrite(led.hardware.pin, val);
  led.patternIdx = (led.patternIdx + 1) % led.patternLength;
}

void tryPatternLED(struct PatternLED& led, uint32_t currentMicros) {
  if(currentMicros - led.prevMicros >= led.interval) {
    led.prevMicros = currentMicros;
    const uint32_t val = led.slots[led.slotIdx];
    led.slotIdx = (led.slotIdx + 1) % SLOTS;

    if(val == 1) {
      // meaningless light slot
      digitalWrite(led.hardware.pin, 1);
    } else {
      // output real data
      stepLED(led);
    }
  }
}

void updateLDR(struct LDR& ldr) {
  ldr.val = analogRead(ldr.pin);
  // calibrate, and slowly squeeze
  ldr.minVal = min(ldr.minVal, ldr.val) + 1;
  ldr.maxVal = max(ldr.maxVal, ldr.val) - 1;

  // ensure that are min and max aren't too close
  const uint32_t minRange = 10;
  const uint8_t tooClose = ldr.minVal + minRange >= ldr.maxVal;

  ldr.minVal = tooClose ? ldr.minVal - (minRange / 2) : ldr.minVal;
  ldr.maxVal = tooClose ? ldr.maxVal + (minRange / 2) : ldr.maxVal;
}

uint8_t getLDRBinary(struct LDR& ldr) {
  const uint32_t midpoint = (ldr.maxVal + ldr.minVal) / 2;
  return ldr.val >= midpoint;
}

void tryLDR(struct LDR& ldr, uint32_t currentMicros) {
  if(currentMicros - ldr.prevMicros >= ldr.interval) {
    ldr.prevMicros = currentMicros;

    updateLDR(ldr);
    const int8_t ldrBin = getLDRBinary(ldr);

    digitalWrite(outputPin, ldrBin);

    if(false && printEnabled) {
      Serial.print("ldr val: ");
      Serial.print(ldr.val);
      Serial.print("\t");

      Serial.print("ldr min: ");
      Serial.print(ldr.minVal);
      Serial.print("\t");

      Serial.print("ldr max: ");
      Serial.print(ldr.maxVal);
      Serial.print("\t");

      Serial.print("ldr bin: ");
      Serial.print(ldrBin);
      Serial.print("\n");
    }
  }
}

void loop() {
  // Read and print input value every 10 microseconds
  const uint32_t currentMicros = micros();

  tryLDR(ldr, currentMicros);
  // tryBlinkLED(led, currentMicros);
  tryPatternLED(led, currentMicros);
}