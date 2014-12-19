/*
 * SetTimeHelper.h
 *
 *  Created on: Nov 28, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */


#ifndef TEENSYTIMEMANAGER_H_
#define TEENSYTIMEMANAGER_H_

#include <Time.h>
#include <DS1307RTC.h>

typedef void(*timeCallback)(tmElements_t);

class SetTimeHelper {
public:
    SetTimeHelper();
    bool setTimeToCompileTime();
    bool setTimeTo(uint8_t h, uint8_t m);
    bool setTimeTo(tmElements_t tm);

private:
    tmElements_t tm;
    char *monthNames[12];
    bool getCompileDate();
    bool getCompileTime();
};

#endif /* TIMEMANAGER_H_ */

