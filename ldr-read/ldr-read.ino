struct LEDHardware {
  uint8_t pin;
  uint8_t state;
};

struct BlinkLED {
  LEDHardware hardware;
  uint32_t prevMillis;
  uint32_t interval;
};

struct LDR {
  uint8_t pin;
  uint32_t prevMillis;
  uint32_t interval;
  uint32_t val;
  uint32_t minVal;
  uint32_t maxVal;
};

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

struct BlinkLED led;
struct LDR ldr;

void setup() {
  Serial.begin(9600); // sets the serial port to 9600
  led = {
    .hardware = { .pin = 13, .state = 0 },
    .prevMillis = 0,
    .interval = 500,
  };
  pinMode(led.hardware.pin, OUTPUT);

  ldr = { .pin = 0, .prevMillis = 0, .interval = 10 };
  pinMode(ldr.pin, INPUT);
}

void toggleLED(struct LEDHardware& led) {
  led.state = abs(1 - led.state);
  digitalWrite(led.pin, led.state);
}

void tryBlinkLED(struct BlinkLED& led, uint32_t currentMillis) {
  if(currentMillis - led.prevMillis >= led.interval) {
    led.prevMillis = currentMillis;
    toggleLED(led.hardware);
  }
}

void tryLDR(struct LDR& ldr, uint32_t currentMillis) {
  if(currentMillis - ldr.prevMillis >= ldr.interval) {
    updateLDR(ldr);
    Serial.print(ldr.val);
    Serial.print("\n");
  }
}

void loop() {
  // Read and print input value every 10 milliseconds
  const uint32_t currentMillis = millis();

  tryLDR(ldr, currentMillis);
  tryBlinkLED(led, currentMillis);
}