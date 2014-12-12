/*
 * CompileTimeManager.h
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

typedef void(*timeCallback)(tmElements_t);

class CompileTimeManager {
public:
    CompileTimeManager(timeCallback setTimeCallback);
    bool setTimeToCompileTime();
    bool setTimeTo(uint8_t h, uint8_t m);

private:
    tmElements_t tm;
    timeCallback setTimeCallback;
    char *monthNames[12];
    bool getTime(const char *str);
    bool getDate(const char *str);
};

#endif /* TIMEMANAGER_H_ */

