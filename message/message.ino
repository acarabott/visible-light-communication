class LED {

  protected:
    uint8_t pin;
    uint8_t state;

  public:
  LED(uint8_t _pin) {
    pin = _pin;
    pinMode(pin, OUTPUT);
  }

  void set(uint8_t _val) {
    state = _val;
    digitalWrite(pin, state);
  }

  uint8_t getState() { return state; }
  uint8_t getPin() { return pin; }
  void setPin(uint8_t _pin) { pin = _pin; }

};

LED led(13);

void setup()
{
}

void loop()
{
  led.set(1);
  delay(500);
  led.set(0);
  delay(500);
}