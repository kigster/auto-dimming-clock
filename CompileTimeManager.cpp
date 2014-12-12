/*
 * CompileTimeManager.cpp
 *
 *  Created on: Nov 28, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#include "CompileTimeManager.h"

#include <Time.h>
#include <Arduino.h>
static const char *monthNamesList[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

CompileTimeManager::CompileTimeManager(timeCallback timeCallback) {
    setTimeCallback = timeCallback;
    for (int i = 0; i < 12; i ++) { monthNames[i] = (char *)monthNamesList[i]; }
}

bool CompileTimeManager::setTimeToCompileTime() {
    // get the date and time the compiler was run
    if (getDate(__DATE__) && getTime(__TIME__)) {
        // and configure the RTC with this info
        time_t compileTime = makeTime(tm) + 30;
        setTime(compileTime);
        if (setTimeCallback != NULL)
            setTimeCallback(tm);
        return true;
    }
    return false;
}

bool CompileTimeManager::getTime(const char *str) {
    int Hour, Min, Sec;
    if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3)
        return false;
    tm.Hour = Hour;
    tm.Minute = Min;
    tm.Second = Sec;
    return true;
}

bool CompileTimeManager::getDate(const char *str) {
    char Month[12];
    int Day, Year;
    uint8_t monthIndex;

    if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3)
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
