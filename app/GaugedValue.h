/*
 * GaugedValue.h
 *
 *  Created on: August 8th, 2015, next to my boo.
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2015 All rights reserved, MIT License.
 */

#ifndef GAUGEDVALUE_H_
#define GAUGEDVALUE_H_
#include "../Wallock.h"

const static int PRINT_WAIT_MS = 100;

namespace Wallock {
    class GaugedValue {
    private:
        unsigned int current;
        unsigned int lastValue;
        unsigned int max;
        unsigned int min;
        unsigned int increment;
        unsigned long lastChangedEpoch;
        unsigned long lastPrintedEpoch;
        bool changed;
        const char *name;
    public:
        GaugedValue    (const char *_name,
                        unsigned int _min,
                        unsigned int _max,
                        unsigned int _increment) {

            name        = (char *)_name;
            min         = _min;
            max         = _max;
            increment   = _increment;

            changed     = false;
            current     = (max - min) / 2;
            lastChangedEpoch = lastPrintedEpoch = current = lastValue = 0;
        }
        signed int getCurrent() {
            return current;
        }

        bool setCurrent(signed int newValue) {
            changed = false;

            // if the difference exceeds increment reduce down to increment itself.
            // this allows to cap any "jumps" in values
            signed int delta = newValue - lastValue;
            if (abs(delta) < increment) {
                // insufficient change, so no change at all
                newValue = lastValue;
            }

            if (newValue      >= (signed int) max)        newValue = max;
            else if (newValue <= (signed int) min)        newValue = min;

            if ((unsigned int) newValue != lastValue) {
                changed = true;

                lastValue = current;
                current = (unsigned int) newValue;
                lastChangedEpoch = millis();
                sprintf(buffer, "%s: %4d --> %4d", name, lastValue, newValue);
                Serial.println(buffer);
            }

            return changed;
        }
        float getCurrentAsPercentOfRange() {
            return ((float) 100.0 * current / (float) (max - min));
        }
        bool setCurrentAsPercentOfRange(float newCurrentValuePercent) {
            return setCurrent((signed int) (newCurrentValuePercent / 100.0 * (float) (max - min)));
        }
        float getLastChangeAsPercentOfRange() {
            return (100.0 * ((float) current - (float) lastValue) / (float) (max - min));
        }
        bool changeBy(signed int delta) {
            return setCurrent((signed int) current + delta);
        }
        bool changeByPercent(float deltaPerc) {
            return changeBy((signed int) floor((float) (max - min) * (deltaPerc / 100.0)));
        }
        bool follow(GaugedValue *another) {
            return setCurrentAsPercentOfRange(another->getCurrentAsPercentOfRange());
        }
        bool follow(GaugedValue *another, float offsetPercentage) {
            float newPercentageValue = another->getCurrentAsPercentOfRange() + offsetPercentage;
            newPercentageValue = max(min(newPercentageValue, 100.0), 0);
            return setCurrentAsPercentOfRange(newPercentageValue);
        }
    };
};
#endif /* GAUGEDVALUE_H_ */
