/*
 * Wallock.h – feature flags
 *
 *  Created on: Dec 19, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#ifndef _WALLOCK_H_
#define _WALLOCK_H_

#define ENABLE_PHOTORESISTOR   true
#define ENABLE_SET_TIME        true
#define ENABLE_MENU            true
#define ENABLE_LCD             false
#define ENABLE_NEOPIXELS       false

#define DEBUG                  false

#define BRIGHTNESS_MAX          15
#define BRIGHTNESS_MIN           0

#include "Arduino.h"

#include <RotaryEncoderWithButton.h>
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include <OneButton.h>
#include <SimpleTimer.h>

extern char buffer[];
extern uint32_t freeRam();
extern SimpleTimer timer;
#endif // _WALLOC_H_


