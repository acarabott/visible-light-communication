const uint8_t inputPin = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(inputPin, INPUT);
}

void loop()
{
  const uint32_t reading = analogRead(inputPin);
  const uint8_t binary = reading > 512;
  Serial.write(binary);
  delay(20);
}