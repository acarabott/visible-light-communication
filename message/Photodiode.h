#ifndef PHOTODIODE_H
#define PHOTODIODE_H
#endif

#include <Arduino.h>

class Photodiode {
protected:
  uint8_t pin;
  uint16_t val = 0;
  uint16_t minVal = 1024;
  uint16_t maxVal = 0;
  uint16_t minRange = 10;

public:
  Photodiode(uint8_t _pin) {
    pin = _pin;
    pinMode(pin, INPUT);
  }


  void update() {
    // slowly squeeze input range
    minVal++;
    const uint16_t reduction = map(maxVal, val, 1023, 1, (maxVal - val) * 0.1);
    maxVal -= reduction;

    val = analogRead(pin);

    // calibrate
    minVal = max(min(minVal, val), 0);
    maxVal = min(max(maxVal, val), 1023);

    // ensure that are min and max aren't too close
    const uint8_t tooClose = minVal + minRange >= maxVal;

    minVal = tooClose ? minVal - (minRange / 2) : minVal;
    maxVal = tooClose ? maxVal + (minRange / 2) : maxVal;
  }

  uint16_t getVal()    { return val; }
  uint8_t  getBinary() {
    return val >= minVal + ((maxVal - minVal) * 0.1); // greather than 10% range
  }
  uint8_t  getPin()    { return pin; }
  uint16_t getMinVal() { return minVal; }
  uint16_t getMaxVal() { return maxVal; }
};
