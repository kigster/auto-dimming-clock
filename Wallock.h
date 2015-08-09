/*
 * Wallock.h – feature flags
 *
 *  Created on: Dec 19, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#ifndef _BEDTIME_H_
    #define _BEDTIME_H_

    #define ENABLE_PHOTORESISTOR
    #define ENABLE_SET_TIME
    #define ENABLE_MENU
    //#define ENABLE_LCD
    //#define ENABLE_NEOPIXELS
#endif

#include "Arduino.h"

#include <RotaryEncoderWithButton.h>
#include <SimpleTimer.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include <OneButton.h>


extern char buffer[];

