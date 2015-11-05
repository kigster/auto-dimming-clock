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

#if ENABLE_SET_TIME


#include <Time.h>
#include <Arduino.h>

static const char *monthNamesList[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

SetTimeHelper::SetTimeHelper() {
    for (int i = 0; i < 12; i ++) { monthNames[i] = (char *)monthNamesList[i]; }
    breakTime(now(), currentTime);
    retrieveCompileTime();
}

bool SetTimeHelper::changeTimeToCompileTimeIfNeeded() {
    time_t tt_now = makeTime(currentTime);
    time_t tt_compiled = makeTime(compiledAt);
    if (tt_now < tt_compiled) {
        setTimeToCompileTime();
        return true;
    }

    return false;
}

bool SetTimeHelper::setTimeToCompileTime() {
    // get the date and time the compiler was run
    if (retrieveCompileTime()) {
        return setTimeTo(compiledAt, millis() + COMPILE_TIME_AND_BOOTLOADER_DELAY);
    }
    return false;
}

bool SetTimeHelper::setTimeTo(tmElements_t tm, time_t offset) {
    time_t timeToSet = makeTime(tm);
    setTime(timeToSet);
#if TEENSYDUINO
    Teensy3Clock.set(timeToSet);
    return true;
#else
    return (bool) RTC.write(tm);
#endif
}

// private

bool SetTimeHelper::retrieveCompileTime() {
    int Hour, Min, Sec;
    if (sscanf(__TIME__, "%d:%d:%d", &Hour, &Min, &Sec) != 3)
        return false;

    compiledAt.Hour = Hour;
    compiledAt.Minute = Min;
    compiledAt.Second = Sec;

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
    compiledAt.Day = Day;
    compiledAt.Month = monthIndex + 1;
    compiledAt.Year = CalendarYrToTm(Year);

    return true;
}


#endif
