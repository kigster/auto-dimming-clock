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
        int previous;
        int rangeEnd;
        int rangeStart;
        int increment;
        bool lockChangeToSingleIncrement;
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
            rangeStart  = _min;
            rangeEnd    = _max;
            increment   = _minimumIncrement;
            lockChangeToSingleIncrement = _lockChangeToSingleIncrement;

            changed     = false;
            current     = (rangeEnd - rangeStart) / 2;
            previous    = 0;
            offset      = 0;

        }
        void ensureRange(int *value) {
            *value = INRANGE(*value, rangeStart, rangeEnd);
        }

        // returns a possibly adjusted by offset value
        signed int getCurrent() {
            return INRANGE(current + offset, rangeStart, rangeEnd);
        }

        int getRangeEnd() {
            return rangeEnd;
        }
        int getRangeStart() {
            return rangeStart;
        }

        float getCurrentAsPercentOfRange() {
            float perc = ((float) getCurrent() - rangeStart) / ((float)rangeEnd - rangeStart) * 100.0;
            perc = INRANGE(perc, 0, 100);
            return perc;
        }

        bool setCurrentFromPercentage(float perc) {
            perc = INRANGE(perc, 0, 100);
            int newValue = (int)((perc / 100.0) * ((float) rangeEnd - rangeStart) + rangeStart);
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
                } else {
                    // make it even number of increments closest to delta
                    delta = ((int) delta / (int) increment) * (int) increment;
                }

                newValue = current + delta;
                ensureRange(&newValue);

                if (newValue != current) {
                    previous = current;
                    current = (int) newValue;
                    changed = true;
//                    log();
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
            offset = percentOffset / 100.0 * (signed int)(rangeEnd - rangeStart);
        }

        void follow(GaugedValue *another) {
            float percAnother = another->getCurrentAsPercentOfRange();
            setCurrentFromPercentage(percAnother);
        }

        void log() {
            #if DEBUG
                sprintf(buffer, "Value Change: %s: %3d->%3d [offset=%3d cur=%3d%%] {%3d...%3d | +-%d}",
                                name,
                                previous,
                                current,
                                offset,
                                (int) floor(getCurrentAsPercentOfRange()),
                                rangeStart,
                                rangeEnd,
                                increment);
                Serial.println(buffer);
                Serial.flush();
            #endif
        }

    };
};
#endif /* GAUGEDVALUE_H_ */
