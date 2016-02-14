const uint8_t inputPin = 0;
const uint8_t outputPin = 13;

void setup()
{
  // Serial.begin(115200);
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
}

void loop()
{
  const uint32_t reading = analogRead(inputPin);
  const uint8_t binary = reading > 512;
  // Serial.write(binary);
  digitalWrite(outputPin, binary);
  delay(20);
}