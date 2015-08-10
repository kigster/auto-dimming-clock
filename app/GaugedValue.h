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
        signed int current;
        signed int lastValue;
        signed int max;
        signed int min;
        signed int increment;
        bool changed;
        unsigned long lastChangedEpoch;
        unsigned long lastPrintedEpoch;
        const char * name;
    public:
        GaugedValue(const char *_name, unsigned short _min, unsigned short _max, unsigned int _increment) {
            name        = (char *)_name;
            setMinMax(_min, _max);

            changed     = false;
            current     = (max - min) / 2;
            increment   = _increment;

        }
        signed int getCurrent() {
            return current;
        }

        bool setCurrent(signed int newValue) {
            changed = false;

            // if the difference exceeds increment reduce down to increment itself.
            // this allows to cap any "jumps" in values
            if (abs(newValue - lastValue) > abs(increment)) {
                // too large of a change, default to our increment.
                signed short sign =  (signed short) abs(newValue) / newValue;
                newValue = lastValue + sign * increment;
            } else if (abs(newValue - lastValue) < abs(increment)) {
                // insufficient change, so no change at all
                newValue = lastValue;
            }

            if (newValue > max)             newValue = max;
            else if (newValue < min)        newValue = min;

            if (newValue != lastValue) {
                changed = true;

                lastValue = current;
                current = newValue;

                sprintf(buffer, "%s: %4d --> %4d", name, lastValue, newValue);
                Serial.println(buffer);
            }

            return changed;
        }
        void setMinMax(signed int  _min, signed int  _max) {
            min = _min;
            max = _max;
        }
        void setMin(signed int  _min) {
            min = _min;
        }
        void setMax(signed int  _max) {
            max = _max;
        }
        float getCurrentPerc() {
            return ((float) 100.0 * current / (float) (max - min));
        }
        float lastDeltaPercent() {
            return ((float) 100.0 * ((float) current - lastValue) / (float) (max - min));
        }

        bool addDeltaToCurrent(signed int delta) {
            return setCurrent((signed int) current + delta);
        }
        bool applyDeltaPercent(float deltaPerc) {
            return addDeltaToCurrent(( (signed int) (((float) max - min) * deltaPerc / 100.0  )));
        }
        bool applyMyDeltaTo(GaugedValue *another) {
            return another->applyDeltaPercent(this->lastDeltaPercent());
        }
    };
};
#endif /* GAUGEDVALUE_H_ */
