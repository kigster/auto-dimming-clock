/*
 * WallClock.h
 *
 *  Created on: Dec 11, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#ifndef WALLCLOCK_H_
#define WALLCLOCK_H_


static signed short h, m, brightness;

typedef void(*displayUpdateCallback)(void);

namespace SetTime {
typedef enum TimeChangeMode_e {
    Default     = (1 << 0),
    Hour        = (1 << 1),
    Minute      = (1 << 2),
    Save        = (1 << 4),
    Last        = (1 << 5),
} TimeMode;
}
;
SetTime::TimeMode mode = SetTime::Default;



#endif /* WALLCLOCK_H_ */
