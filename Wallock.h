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

#include <Arduino.h>

#define ENABLE_PHOTORESISTOR        true
#define ENABLE_SET_TIME             true
#define ENABLE_MENU                 true
#define ENABLE_LCD                  false
#define ENABLE_ENCODER_RGB          false
#define ENABLE_NEOPIXELS            false
#define ENCODER_USE_INTERRUPTS      true

#define BRIGHTNESS_MAX              15
#define BRIGHTNESS_MIN              0

#define ENCODER_BTN_ACTIVE_LOW      0
#define ENCODER_RGB_OFF_STATE       HIGH

#define DEBUG                       false
#define ANALOG_OUTPUT_RESOLUTION    256
#define LOAD_COLOR_NAMES            true


#define CYCLE_COLOR_THEME_ONE`

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


