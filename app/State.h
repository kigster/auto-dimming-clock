/*
 * BedTimeState.h
 *
 *  Created on: August 8th, 2015, next to my boo.
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2015 All rights reserved, MIT License.
 */

#ifndef BEDTIME_STATE_H_
#define BEDTIME_STATE_H_
#include "GaugedValue.h"

class State {
private:
    GaugedValue *outsideBrightness, *displayBrightness;

    bool colonOn = false, displayOn = false, neoPixelsOn = false;
public:
    State(GaugedValue *_outsideBrightness, GaugedValue *_displayBrightness) {
        outsideBrightness = _outsideBrightness;
        displayBrightness = _displayBrightness;
    }

    void setPhotoresistorValue(uint16_t photoValue) {
        outsideBrightness->setCurrent(photoValue);
    }
    GaugedValue *getOutsideBrightness() {
        return outsideBrightness;
    }
    GaugedValue *getDisplayBrightness() {
        return displayBrightness;
    }


};

#endif /* BEDTIME_STATE_H_ */
