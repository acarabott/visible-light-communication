struct LEDHardware {
  uint8_t pin;
  uint8_t state;
};

struct BlinkLED {
  LEDHardware hardware;
  uint32_t prevMicros;
  uint32_t interval;
};

struct LDR {
  uint8_t pin;
  uint32_t prevMicros;
  uint32_t interval;
  uint32_t val;
  uint32_t minVal;
  uint32_t maxVal;
};

const uint8_t printEnabled = 0;

struct BlinkLED led;
struct LDR ldr;

void setup() {
  if(printEnabled) {
    Serial.begin(9600); // sets the serial port to 9600
  }

  led = {
    .hardware = { .pin = 13, .state = 0 },
    .prevMicros = 0,
    .interval = 20,
  };
  pinMode(led.hardware.pin, OUTPUT);

  ldr = {
    .pin = 0,
    .prevMicros = 0,
    .interval = 20,
    .val = 0,
    .minVal = 9999,
    .maxVal = 0,
  };
  pinMode(ldr.pin, INPUT);
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

void updateLDR(struct LDR& ldr) {
  ldr.val = analogRead(ldr.pin);
  // calibrate, and slowly squeeze
  ldr.minVal = min(ldr.minVal, ldr.val) + 1;
  ldr.maxVal = max(ldr.maxVal, ldr.val) - 1;

  // ensure that are min and max aren't too close
  const uint32_t minRange = 5;
  ldr.maxVal = ldr.maxVal < ldr.minVal + minRange ? ldr.maxVal + minRange :
                                                    ldr.maxVal;
}

uint8_t getLDRBinary(struct LDR& ldr) {
  const uint32_t midpoint = (ldr.maxVal + ldr.minVal) / 2;
  return ldr.val >= midpoint;
}

void tryLDR(struct LDR& ldr, uint32_t currentMicros) {
  if(currentMicros - ldr.prevMicros >= ldr.interval) {
    ldr.prevMicros = currentMicros;
    updateLDR(ldr);

    if(printEnabled) {
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
      Serial.print(getLDRBinary(ldr));
      Serial.print("\n");
    }
  }
}

void loop() {
  // Read and print input value every 10 microseconds
  const uint32_t currentMicros = micros();

  tryLDR(ldr, currentMicros);
  tryBlinkLED(led, currentMicros);
}