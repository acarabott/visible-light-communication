void setup() {
  Serial.begin(9600); // sets the serial port to 9600
}

void loop() {
  const int val = analogRead(0); // read analog input pin 0
  Serial.print(val); // prints the value read
  Serial.print("\n"); // prints a space between the numbers
  delay(10); // wait 10ms for next reading
}