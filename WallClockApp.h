/*
 * WallClockApp.h
 *
 *  Created on: Dec 19, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#ifndef WALLCLOCKAPP_H_
#define WALLCLOCKAPP_H_

#include <Arduino.h>
#include <RotaryEncoderWithButton.h>
#include <SimpleTimer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Time.h>
#include <DS1307RTC.h>

#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include <OneButton.h>
#include "SetTimeHelper.h"
#include "SetTimeMenu.h"
#include "WallClock.h"

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
typedef struct hardwareConfig_s {
    uint8_t pinLED;
    uint8_t pinPhotoResistor;
    uint8_t pinRedButton;
    uint8_t pinRotaryLeft;
    uint8_t pinRotaryRight;
    uint8_t pinRotaryButton;
} HardwareConfig;


class SetTimeMenu;

class WallClockApp {
public:
    WallClockApp(HardwareConfig config);

    OneButton *rotaryButton,    *redButton;
    Adafruit_7segment           *matrix;
    RotaryEncoderWithButton     *rotary;
    LiquidCrystal_I2C           *lcd;  // set the LCD address to 0x27 for a 16 chars and 2 line display
    SimpleTimer                 *timer;
    SetTime::TimeMode           mode;
    uint8_t                     brightness;
    SetTimeMenu                 *menu;
    SetTimeHelper               *helper;
    char buf[30];
    HardwareConfig config;
    bool colonOn, screenOn;

    void setup();
    void loop();
    void updateBrightness();
    void buttonClick();
    void displayTime(signed short h, signed short m);
    void blinkLED();
    void debug(int row, const char *message, bool clear);
    void debug(const char *message);
    void cb_RotaryButtonClick();
    void cb_RotaryButtonDoubleClick();
    void cb_RotaryButtonHold();
    void cb_ToggleDisplay();
    void cb_ReadPhotoResistor();
    void cb_DisplayTimeNow();
private:
};

#endif /* WALLCLOCKAPP_H_ */
