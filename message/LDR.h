#ifndef LDR_H
#define LDR_H
#endif

#include <Arduino.h>

class LDR {
protected:
  uint8_t pin;
  uint16_t val = 0;
  uint16_t minVal = 1024;
  uint16_t maxVal = 0;
  uint16_t minRange = 10;

public:
  LDR(uint8_t _pin) {
    pin = _pin;
    pinMode(pin, INPUT);
  }


  void update() {
    val = analogRead(pin);

    // calibrate, and slowly squeeze
    minVal = min(minVal, val) + 1;
    maxVal = max(maxVal, val) - 1;

    // ensure that are min and max aren't too close
    const uint8_t tooClose = minVal + minRange >= maxVal;

    minVal = tooClose ? minVal - (minRange / 2) : minVal;
    maxVal = tooClose ? maxVal + (minRange / 2) : maxVal;
  }

  uint16_t getVal() {
    return val;
  }

  uint8_t getBinary() {
    return val >= (maxVal + minVal) / 2;
  }
  uint8_t getPin() { return pin; }
  uint16_t getMinVal() { return minVal; }
  uint16_t getMaxVal() { return maxVal; }
};
