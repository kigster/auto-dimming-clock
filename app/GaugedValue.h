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
        bool lockChangeToSingleIncrement;
        unsigned long lastChangedEpoch;
        unsigned long lastPrintedEpoch;
        bool changed;
        const char *name;
        float percentOffset;
    public:
        GaugedValue    (const char *_name,
                        unsigned int _min,
                        unsigned int _max,
                        unsigned int _increment,
                        bool _lockChangeToSingleIncrement) {

            name        = (char *)_name;
            min         = _min;
            max         = _max;
            increment   = _increment;
            lockChangeToSingleIncrement = _lockChangeToSingleIncrement;

            changed     = false;
            current     = (max - min) / 2;
            lastChangedEpoch = lastPrintedEpoch = current = lastValue = 0;
            percentOffset = 0.0;

        }
        signed int getCurrent() {
            return current;
        }

        bool setCurrent(signed int newValue) {
            changed = false;

            // if the difference exceeds increment reduce down to increment itself.
            // this allows to cap any "jumps" in values
            signed int delta = newValue - current;
            if (abs(delta) < increment) {
                // insufficient change, so no change at all
                return false;
            } else if (abs(delta) >= increment && lockChangeToSingleIncrement) {
                newValue = current + (delta/abs(delta)) * increment;
            }

            if (newValue      >= (signed int) max)        newValue = max;
            else if (newValue <= (signed int) min)        newValue = min;

            if ((unsigned int) newValue != current && millis() - lastChangedEpoch > 50) {
                changed = true;
                lastValue = current;
                current = newValue;
                lastChangedEpoch = millis();
                sprintf(buffer, "%s: [%4d]-->[%4d] offset [%4d] ", name, lastValue, current, (short) floor(percentOffset));
                Serial.println(buffer);
            }

            return changed;
        }
        float getCurrentAsPercentOfRange() {
            return ((float) 100.0 * current / (float) (max - min));
        }
        float getCurrentAsPercentOfRangeWithOffset() {
            return max(min(((float) 100.0 * (current - min) / (float) (max - min) - percentOffset), 100), 0);
        }
        bool setCurrentAsPercentOfRange(float newValue) {
            return setCurrent( (signed int) ((newValue) / 100.0 * (float) (max - min)));
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
        void syncTo(GaugedValue *another) {
            percentOffset = getCurrentAsPercentOfRange() - another->getCurrentAsPercentOfRange();
            percentOffset = max(min(percentOffset, 100.0), -100);
        }
        bool follow(GaugedValue *another) {
            return setCurrentAsPercentOfRange(another->getCurrentAsPercentOfRangeWithOffset());
        }
    };
};
#endif /* GAUGEDVALUE_H_ */
