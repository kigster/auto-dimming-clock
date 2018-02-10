/*
 * SetTimeHelper.h
 *
 *  Created on: Nov 28, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#include "Wallock.h"

#if ENABLE_SET_TIME

#ifndef TEENSYTIMEMANAGER_H_
#define TEENSYTIMEMANAGER_H_

#define COMPILE_TIME_AND_BOOTLOADER_DELAY 40000

#include <Time.h>
#include <DS1307RTC.h>

//typedef void(*timeCallback)(tmElements_t);

class SetTimeHelper {
public:
    SetTimeHelper();
    bool setTimeToCompileTime();
    bool setTimeTo(uint8_t h, uint8_t m);
    bool setTimeTo(tmElements_t tm, time_t offset);
    bool changeTimeToCompileTimeIfNeeded();
    tmElements_t currentTime;
    tmElements_t compiledAt;
private:
    char *monthNames[12];
    bool retrieveCompileTime();
//    time_t currentDateTime();
//    time_t compileDateTime();
};

#endif /* TIMEMANAGER_H_ */
#endif /* ENABLE_SET_TIME */
