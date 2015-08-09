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
#include "../BedTime.h"

const static int PRINT_WAIT_MS = 1000;

class GaugedValue {
private:
    signed int current;
    signed int lastValue;
    signed int max;
    signed int min;
    unsigned int lastSavedMs;
    const char * name;
public:
    GaugedValue(const char *_name, unsigned short _min, unsigned short _max) {
        name = (char *)_name;
        min = _min;
        max = _max;
        current = 0;
        setMinMax(0, 0);
    }
    bool isInitialized() {
        return !(abs(current) + abs(min) + abs(max) == 0);
    }

    signed int  getCurrent() {
        return current;
    }

    void setCurrent(signed int value) {
        int _lastValue = lastValue;
        uint32_t rightNow = now();
        if (value <= max && value >= min) {
            lastValue = current;
            current = value;
            if (rightNow - lastSavedMs > PRINT_WAIT_MS) {
                sprintf(buffer, "[value changed] %s: %4d --> %4d", name, _lastValue, value);
                Serial.println(buffer);
                lastSavedMs = rightNow;
            }
        }
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
        return ((float) 100.0 * (current - lastValue) / (max - min));
    }
    void addDeltaToCurrent(signed int delta) {
        setCurrent(current += delta);
    }
    void applyDeltaPercent(float deltaPerc) {
        addDeltaToCurrent((deltaPerc / 100.0 * (max - min) ));
    }
    void applyMyDeltaTo(GaugedValue *another) {
        another->applyDeltaPercent(this->lastDeltaPercent());
    }
};

#endif /* GAUGEDVALUE_H_ */
