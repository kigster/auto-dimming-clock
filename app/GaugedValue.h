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


#define INRANGE(X, MIN, MAX) (min(max(X, MIN), MAX))


const static int PRINT_WAIT_MS = 100;

namespace Wallock {
    class GaugedValue {
    private:
        int current;
        int lastValue;
        int max;
        int min;
        int increment;
        bool lockChangeToSingleIncrement;
        long lastChangedEpoch;
        long lastPrintedEpoch;
        bool changed;
        const char *name;

    public:
        int offset;

        GaugedValue    (const char *_name,
                        int _min,
                        int _max,
                        int _minimumIncrement,
                        bool _lockChangeToSingleIncrement) {

            name        = (char *)_name;
            min         = _min;
            max         = _max;
            increment   = _minimumIncrement;
            lockChangeToSingleIncrement = _lockChangeToSingleIncrement;

            changed     = false;
            current     = (max - min) / 2;
            lastChangedEpoch = lastPrintedEpoch = current = lastValue = 0;
            offset      = 0;

        }
        void ensureRange(int *value) {
            *value = INRANGE(*value, min, max);
        }

        // returns a possibly adjusted by offset value
        signed int getCurrent() {
            return INRANGE(current + offset, min, max);
        }

        float getCurrentAsPercentOfRange() {
            float perc = ((float) getCurrent() - min) / ((float)max - min) * 100.0;
            perc = INRANGE(perc, 0, 100);
            return perc;
        }

        bool setCurrentFromPercentage(float perc) {
            perc = INRANGE(perc, 0, 100);
            int newValue = (int)((perc / 100.0) * ((float) max - min) + min);
            if (current != newValue) {
                current = newValue;
                return true;
            }
            return false;
        }

        bool changeCurrentBy(int delta) {
            bool changed = false;
            int newValue;

            if (abs(delta) >= increment) {
                if (lockChangeToSingleIncrement) {
                    delta = delta > 0 ? increment : -increment;
                }

                newValue = current + delta;
                ensureRange(&newValue);

                if (newValue != current) {
                    lastValue = current;
                    current = (int) newValue;
                    lastChangedEpoch = millis();
                    changed = true;
                    toSerial();
                }
            }

            return changed;
        }

        bool setCurrent(int newValue) {
            ensureRange(&newValue);
            int delta = newValue - current;
            return changeCurrentBy(delta);
        }

        void syncOffsetTo(GaugedValue *another) {
            float percentOffset = another->getCurrentAsPercentOfRange() -
                                     this->getCurrentAsPercentOfRange();
            percentOffset = INRANGE(percentOffset, -50.0, 50.0); // max shift is half the range
            offset = percentOffset / 100.0 * (signed int)(max - min);
        }

        void follow(GaugedValue *another) {
            float percAnother = another->getCurrentAsPercentOfRange();
            setCurrentFromPercentage(percAnother);
        }

        void toSerial() {
            #if DEBUG
                sprintf(buffer, " <%s: (%3d)->(%3d|%3d) %3d%%> ",
                                name,
                                lastValue,
                                current,
                                offset,
                                (int) floor(getCurrentAsPercentOfRange()));
                Serial.print(buffer);
            #endif
        }

    };
};
#endif /* GAUGEDVALUE_H_ */
