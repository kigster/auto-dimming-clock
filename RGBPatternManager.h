/*
 * RGBPatternManager.h
 *
 *  Created on: Dec 29, 2015
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#ifndef APP_RGBPATTERNMANAGER_H_
#define APP_RGBPATTERNMANAGER_H_
#include "Wallock.h"

#include "RGBController.h"

#ifdef CYCLE_COLOR_THEME_ONE

//const PROGMEM rgb_color_t cycleColors[] = {
//              ColorNames::aqua,
//              ColorNames::palevioletred,
//              ColorNames::maroon,
//              ColorNames::indigo,
//              ColorNames::burlywood,
//              ColorNames::darkgoldenrod,
//              ColorNames::darkred,
//              ColorNames::teal,
//              ColorNames::mediumpurple,
//              ColorNames::deeppink,
//              ColorNames::crimson,
//              ColorNames::magenta,
//              ColorNames::lightsteelblue,
//              ColorNames::olivedrab,
//              ColorNames::orchid,
//              ColorNames::peru,
//              ColorNames::orangered,
//              ColorNames::plum,
//              ColorNames::thistle,
//              ColorNames::turquoise,
//              ColorNames::seagreen,
//};
const PROGMEM rgb_color_t cycleColors[] = {
              ColorNames::yellow,
              ColorNames::red,
              ColorNames::blue
};

const PROGMEM int cycleColorSize = sizeof(cycleColorSize) / sizeof(rgb_color_t);

#endif

namespace Wallock {

  class RGBPatternManager {
    private:
      Wallock::RGBController &_controller;
      int _currentIndex = 0;

    public:
      /**
       * Assume that random number generator was already seeded.
       */
      RGBPatternManager(RGBController &controller) :
              _controller(controller) {
      };

      void fadeToNext(int durationMs) {
        rgb_color_t c1 = cycleColors[_currentIndex++];

        _currentIndex %= cycleColorSize;
        rgb_color_t c2 = cycleColors[_currentIndex];

        getController().fade(c1, c2, durationMs);
      }

    private:
      RGBController &getController() {
        return _controller;
      }
  };
}

#endif /* APP_RGBPATTERNMANAGER_H_ */
