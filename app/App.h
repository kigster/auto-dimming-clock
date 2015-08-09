/*
 * BedTimeApp.h
 *
 *  Created on: Dec 19, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#ifndef BEDTIMEAPP_H_
#define BEDTIMEAPP_H_

#include "../Wallock.h"
#include "State.h"


#ifdef ENABLE_LCD
#include <LiquidCrystal_I2C.h>
#endif

#ifdef ENABLE_SET_TIME
#include "../settingtime/SetTimeHelper.h"
#include "../settingtime/SetTimeMenu.h"
#endif

#ifdef ENABLE_NEOPIXELS
#include "../neopixel/NeoPixelManager.h"
#include "../neopixel/NeoPixelEffects.h"
#endif

#define MAX_BRIGHTNESS 15
#define MIN_BRIGHTNESS 0

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
    uint8_t pinPhotoResistor;
    uint8_t pinRotaryLeft;
    uint8_t pinRotaryRight;
    uint8_t pinRotaryButton;
    uint8_t pinNeoPixels;
    State *state;
} HardwareConfig;

class App {
public:
    App(HardwareConfig *config);

    State                       *state;
    Adafruit_7segment           *matrix;
    SetTime::TimeMode           mode;
#ifdef ENABLE_LCD
    LiquidCrystal_I2C           *lcd;  // set the LCD address to 0x27 for a 16 chars and 2 line display
#endif
#ifdef ENABLE_MENU
    SetTimeMenu                 menu;
#endif
#ifdef ENABLE_SET_TIME
    SetTimeHelper               helper;
#endif
#ifdef ENABLE_NEOPIXELS
    NeoPixelManager             *neoPixelManager;
#endif
    RotaryEncoderWithButton     *rotary;
    HardwareConfig              *config;

    void setup();
    void run();

    void refreshUI();
    void readEnvironment();

    void displayTime(signed short h, signed short m);
    void displayCurrentTime();

    void toggleDisplay();
    void toggleNeoPixels();
    void neoPixelRefresh();
    void neoPixelNextEffect();
    // Callbacks
    void cb_ButtonClick();
    void cb_ButtonDoubleClick();
    void cb_ButtonHold();

    void debug(int row, const char *message, bool clear);
    void debug(const char *message);

    void readPhotoresitor();
    void readKnob();

private:

    bool colonOn, screenOn, neoPixelsOn;
    tmElements_t lastDisplayedTime;
};

#endif /* BEDTIMEAPP_H_ */
