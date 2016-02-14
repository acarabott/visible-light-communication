#ifndef LED_H
#define LED_H
#endif

#include <Arduino.h>

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

  void toggle() {
    set(abs(state - 1));
  }

  uint8_t getState() { return state; }
  uint8_t getPin() { return pin; }
};
