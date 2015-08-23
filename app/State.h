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

    typedef struct StateValues_s {
        bool
            displayOn = true,
            colonOn = true;
        bool
            clock24hr = false,
            neoPixelsOn = false;
        int brightness;
     } StateValues;


    class State {
    private:
        StateValues values;
    public:
        GaugedValue &photoReadout,
                    &brightness;

        State(GaugedValue &g1, GaugedValue &g2) :
            photoReadout(g1),
            brightness(g2) {};

        GaugedValue &getPhotoReadout() {
            return photoReadout;
        }
        GaugedValue &getBrightness() {
            return brightness;
        }
        StateValues getValues() {
            return values;
        }
        void setBrightness(int newVal) {
            brightness.setCurrent(newVal);
            values.brightness = brightness.getCurrent();
        }
        int currentBrightness() {
            return brightness.getCurrent();
        }
        int currentPhotoReadout() {
            return photoReadout.getCurrent();
        }
        void flipColon() {
            values.colonOn = !values.colonOn;
        }
        void flip24Hr() {
            values.clock24hr = !values.clock24hr;
        }
        void flipDisplayOn() {
            values.displayOn = !values.displayOn;
        }
        void synchronize() {

        }
    };
};

#endif /* WALLOCK_STATE_H_ */
