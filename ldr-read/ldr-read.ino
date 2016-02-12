struct LEDHardware {
  uint8_t pin;
  uint8_t state;
};

struct BlinkLED {
  LEDHardware hardware;
  uint32_t prevMillis;
  uint32_t interval;
};

struct BlinkLED led;

const int gLDRPin = 0;
uint32_t gPrevLDRMillis = 0;



void setup() {
  Serial.begin(9600); // sets the serial port to 9600
  led = {
    .hardware = { .pin = 13, .state = 0 },
    .prevMillis = 0,
    .interval = 500,
  };
  pinMode(led.hardware.pin, OUTPUT);
  // pinMode(gLDRPin, INPUT);
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

void tryLDR(uint32_t currentMillis) {
  const uint32_t ldrInterval = 10;
  if(currentMillis - gPrevLDRMillis >= ldrInterval) {
    gPrevLDRMillis = currentMillis;
    const int val = analogRead(gLDRPin);
    Serial.print(val);
    Serial.print("\n");
  }
}

void loop() {
  // Read and print input value every 10 milliseconds
  const uint32_t currentMillis = millis();

  tryLDR(currentMillis);
  tryBlinkLED(led, currentMillis);
}