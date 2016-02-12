const int gLDRPin = 0;
unsigned long gPrevLDRMillis = 0;

const int gLEDPin = 13;
int gLEDState = 0;
unsigned long gPrevLEDMillis = 0;

void setup() {
  Serial.begin(9600); // sets the serial port to 9600
  pinMode(gLEDPin, OUTPUT);
  pinMode(gLDRPin, INPUT);
}

void toggleLED() {
  gLEDState = abs(1 - gLEDState);
  digitalWrite(gLEDPin, gLEDState);
}

void tryLED(unsigned long currentMillis) {
  const unsigned long ledInterval = 500;
  if(currentMillis - gPrevLEDMillis >= ledInterval) {
    gPrevLEDMillis = currentMillis;
    toggleLED();
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
  tryLED(currentMillis);
}