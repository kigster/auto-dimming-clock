/*
 * BedTimeState.h
 *
 *  Created on: August 8th, 2015, next to my boo.
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2015 All rights reserved, MIT License.
 */

#ifndef WALLOCK_STATE_H_
#define WALLOCK_STATE_H_

#include "GaugedValue.h"

namespace Wallock {
    class State {
    private:
        GaugedValue &photoresistorReading,
                    &displayBrightness;

        bool    colonOn = false,
                displayOn = false,
                neoPixelsOn = false;
    public:
        State(GaugedValue &g1, GaugedValue &g2) :
            photoresistorReading(g1),
            displayBrightness(g2) {};

        GaugedValue &getPhotoresistorReading() {
            return photoresistorReading;
        }
        GaugedValue &getDisplayBrightness() {
            return displayBrightness;
        }
    };
};
#endif /* WALLOCK_STATE_H_ */
