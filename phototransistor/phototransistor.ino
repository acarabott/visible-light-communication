const uint8_t photoPin = 1;
uint32_t prevPhotoMicros = 0;
uint32_t photoDur = 10000;

uint32_t prevLedMicros = 0;
uint32_t ledDur = 500000;
uint8_t ledState = 0;
const uint8_t ledPin = 13;

void setup() {
  Serial.begin(9600); // sets the serial port to 9600
  pinMode(photoPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  uint32_t curMicros = micros();

  if(curMicros - prevPhotoMicros >= photoDur) {
    prevPhotoMicros = curMicros;

    const int val = analogRead(photoPin);
    Serial.println(val);
  }

  if(curMicros - prevLedMicros >= ledDur) {
    prevLedMicros = curMicros;
    digitalWrite(ledPin, ledState);
    ledState = abs(ledState - 1);
  }
}