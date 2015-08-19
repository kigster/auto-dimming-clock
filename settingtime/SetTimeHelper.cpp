/*
 * SetTimeHelper.cpp
 *
 *  Created on: Nov 28, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */
#include "SetTimeHelper.h"
#include "../Wallock.h"

#ifdef ENABLE_SET_TIME

#include <Time.h>
#include <Arduino.h>

static const char *monthNamesList[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

SetTimeHelper::SetTimeHelper() {
    for (int i = 0; i < 12; i ++) { monthNames[i] = (char *)monthNamesList[i]; }
    makeTime(tm);

}

bool SetTimeHelper::setDateToCompileTime() {
    // get the date and time the compiler was run
    if (getCompileDate() && getCompileTime()) {
        // and configure the RTC with this info
        tm.Hour = 12;
        tm.Minute = 0;
        return setTimeTo(tm);
    }
    return false;
}

bool SetTimeHelper::getCompileTime() {
    int Hour, Min, Sec;
    if (sscanf(__TIME__, "%d:%d:%d", &Hour, &Min, &Sec) != 3)
        return false;
    tm.Hour = Hour;
    tm.Minute = Min;
    tm.Second = Sec;
    return true;
}

bool SetTimeHelper::getCompileDate() {
    char Month[12];
    int Day, Year;
    uint8_t monthIndex;

    if (sscanf(__DATE__, "%s %d %d", Month, &Day, &Year) != 3)
        return false;

    for (monthIndex = 0; monthIndex < 12; monthIndex++) {
        if (strcmp(Month, monthNames[monthIndex]) == 0)
            break;
    }
    if (monthIndex >= 12)
        return false;
    tm.Day = Day;
    tm.Month = monthIndex + 1;
    tm.Year = CalendarYrToTm(Year);
    return true;
}

bool SetTimeHelper::setTimeTo(tmElements_t tm) {
    time_t timeToSet = makeTime(tm);
    setTime(timeToSet);
#ifdef TEENSYDUINO
    time_t t = makeTime(tm);
    Teensy3Clock.set(t);
    setTime(t);
    return true;
#else
    if (RTC.write(tm)) {
        return true;
    } else {
        return true;
    }
#endif
}

#endif
