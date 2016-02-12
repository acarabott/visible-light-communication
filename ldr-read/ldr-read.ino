struct LEDHardware {
  int pin;
  int state;
};

struct BlinkLED {
  LEDHardware hardware;
  unsigned long prevMillis;
  unsigned long interval;
};

struct BlinkLED led;

const int gLDRPin = 0;
unsigned long gPrevLDRMillis = 0;



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

void tryBlinkLED(struct BlinkLED& led, unsigned long currentMillis) {
  if(currentMillis - led.prevMillis >= led.interval) {
    led.prevMillis = currentMillis;
    toggleLED(led.hardware);
  }
}

void tryLDR(unsigned long currentMillis) {
  const unsigned long ldrInterval = 10;
  if(currentMillis - gPrevLDRMillis >= ldrInterval) {
    gPrevLDRMillis = currentMillis;
    const int val = analogRead(gLDRPin);
    Serial.print(val);
    Serial.print("\n");
  }
}

void loop() {
  // Read and print input value every 10 milliseconds
  const unsigned long currentMillis = millis();

  tryLDR(currentMillis);
  tryBlinkLED(led, currentMillis);
}