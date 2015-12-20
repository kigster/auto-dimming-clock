/*
 * color.h
 *
 *  Created on: Sep 6, 2015
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#ifndef APP_RGBENCODER_H_
#define APP_RGBENCODER_H_

#if ENABLE_ENCODER_RGB

#include <avr/pgmspace.h>
#include "Colors.h"
#include "Arduino.h"

namespace Wallock {

  class RGBEncoder {

    private:
      uint8_t _colors[3] = {0,0,0};
      uint8_t _pins[3];
      uint32_t _lastUpdateAt = 0;
      uint32_t _periodMs = 0;
      bool _blinking = false;
      long _blinkCount = 0, _blinkForCount = 0;
      bool _lastBlinkStateOn = false;

    public:
    RGBEncoder(uint8_t _r, uint8_t _g, uint8_t _b) {
      _pins[0] = _r;
      _pins[1] = _g;
      _pins[2] = _b;
    };

    void setup() {
      for (uint8_t i = 0; i < 3; i++) {
        pinMode(_pins[i], OUTPUT);
      }
      off();
    }

    void tick() {
      _checkTiming();
    }

    void rgbBlink(long color, long periodMs) {
       rgbBlink(color, periodMs, 0);
    }

    // times = 0 for unlimited
    void rgbBlink(long color, long periodMs, long times) {
      _resetTiming();
      _blinkForCount = times;
      _blinking = true;
      _blinkCount = 0;
      _periodMs = periodMs;
      _lastUpdateAt = millis();
      _lastBlinkStateOn = true;
      _rgb(color);
    }

    // show color for a period, then turn off
    void rgbFor(long color, long periodMs) {
      rgbBlink(color, periodMs, 1);
    }

    void rgb(long color) {
      _resetTiming();
      _rgb(color);
    }

    void rgb(uint8_t r, uint8_t g, uint8_t b) {
      _resetTiming();
      _colors[0] = r;
      _colors[1] = g;
      _colors[2] = b;
      _rgb();
    }

    void off() {
      _resetTiming();
      _off();
    }

private:
    void _resetTiming() {
      _blinking = false;
      _blinkForCount = 0 ;
      _blinkCount = 0;
      _periodMs = 0;
      _lastUpdateAt = 0;
      _lastBlinkStateOn = false;
    }

    void _rgb() {
      _write();
    }
    void _rgb(long color) {
      _toRGB(color);
      _write();
    }

    void _off() {
      for (uint8_t i = 0; i < 3; i++) {
        analogWrite(_pins[i], 255);
      }
    }


    void _checkTiming() {
      if (_blinking) {
        if (millis() >= _lastUpdateAt + _periodMs) {
          if (_blinkForCount > 0 && _blinkCount >= _blinkForCount * 2) {
            _resetTiming();
            off();
          } else {
            _blinkCount++;
            _lastBlinkStateOn = !_lastBlinkStateOn;
            _lastBlinkStateOn ? _rgb() : _off();
            _lastUpdateAt = millis();
          }
        }
      }
    }

    void _write() {
      for (uint8_t i = 0; i < 3; i++) {
        analogWrite(_pins[i], 255 - _colors[i]);
      }
    }

    void _toRGB(long color) {
      _colors[0] = (uint8_t)((color & 0xFF0000) >> 020);
      _colors[1] = (uint8_t)((color & 0x00FF00) >> 010);
      _colors[2] = (uint8_t)((color & 0x0000FF));
    }
  };

#endif /* ENABLE_ENCODER_RGB */

} /* namespace Wallock */

#endif /* APP_RGBENCODER_H_ */
