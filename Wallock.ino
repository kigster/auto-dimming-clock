/*
 * BedTime
 *
 * 7-Segment Display for current time, based on the RTC chip.
 *
 * Rotary encoder is used to change s, and set time. The button
 * on rotary encoder is used in single click, double click and hold capacity
 * for various options. See README for more info.
 *
 * Press and hold to enter configuration menu, then click to advance
 * to the next.
 *
 * (c) 2014 All rights reserved.  Please see LICENSE.
 *
 * Author: Konstantin Gredeskoul <kigster@gmail.com>
 */
#include "Wallock.h"
#include "app/App.h"
#include "app/AppInstance.h"
#include "app/RGBController.h"
#include "app/State.h"


char buffer[200];

Wallock::GaugedValue gvBrightness  ("brightness", BRIGHTNESS_MIN, BRIGHTNESS_MAX, 1, true);
Wallock::GaugedValue gvPhotoReadout("photoValue", 60, 360, 20, false);


Wallock::PinoutMapping pinout = {
        A3,     // PhotoResistor
         2,     // Left Rotary
         3,     // Right Rotary
         4,     // Rotary Button
 { 9, 10, 11 }, // RGB LED pins, must support PWM, typically  3, 5, 6, 9, 10, and 11.
         2      // Number of NeoPixels
};

RotaryEncoderWithButton rotary(
                (uint8_t) pinout.pinRotaryLeft,
                (uint8_t) pinout.pinRotaryRight,
                (uint8_t) pinout.pinRotaryButton,
                ENCODER_BTN_ACTIVE_LOW
                );

#if ENABLE_ENCODER_RGB
    Wallock::RGBController colorManager(
            pinout.rgb[0],
            pinout.rgb[1],
            pinout.rgb[2]);
#endif

Adafruit_7segment matrix;
Wallock::State state(gvPhotoReadout, gvBrightness);

#if ENABLE_ENCODER_RGB
    Wallock::App app(pinout, state, rotary, matrix, colorManager);
#else
    Wallock::App app(pinout, state, rotary, matrix);
#endif

SimpleTimer timer;

#if ENABLE_NEOPIXELS
#include "app/NeoPixelEffects.h"
#include "app/NeoPixelManager.h"
#endif

#if TEENSYDUINO
    time_t getTeensy3Time() {
        return Teensy3Clock.get();
    }

    uint32_t freeRam(){ // for Teensy 3.0
        uint32_t stackTop;
        uint32_t heapTop;

        // current position of the stack.
        stackTop = (uint32_t) &stackTop;

        // current position of heap.
        void* hTop = malloc(1);
        heapTop = (uint32_t) hTop;
        free(hTop);

        // The difference is the free, available ram.
        return stackTop - heapTop;
    }
#else
    uint32_t freeRam () {
        extern int __heap_start, *__brkval;
        int v;
        return (uint32_t) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    }
#endif

bool detectRTC() {
    tmElements_t &tm = app.helper.currentTime;
#ifndef TEENSYDUINO
    bool rtcResult = RTC.read(tm);
    if (RTC.chipPresent()) {
        if (!rtcResult) {
            Serial.println(F("RTC.read() returned false, resetting to compile time"));
            return app.helper.setTimeToCompileTime();
        } // the only way to go on is if rtcResult is true
    } else {
        Serial.println(F("RTC chip was not detected."));
        return false;
    }
#else
    breakTime(now(), tm);
#endif

    logTime("CURRENT", app.helper.currentTime);
    logTime("COMPILE", app.helper.compiledAt);

    bool changed = app.helper.changeTimeToCompileTimeIfNeeded();
    if (changed)
        logTime("UPDATED", app.helper.currentTime);

    return true;
}

void logTime(char *timeType, tmElements_t tm) {
    sprintf(buffer,
                    "%s TIME: %d/%d/%d %02d:%02d:%02d | time_t: ",
                    timeType,
                    tm.Month, tm.Day, tmYearToCalendar(tm.Year), tm.Hour, tm.Minute, tm.Second);
    Serial.print(buffer);
    Serial.println( makeTime(tm));
}

void setup() {
    Serial.begin(9600);
#if TEENSYDUINO
    setSyncProvider(getTeensy3Time);
#endif
    Serial.println(F("[wallock] v3.0(c) 2015 kiguino.moos.io"));

    app.setup();

    app.getButton()->attachClick((callbackFunction ) Wallock::callBack_buttonClick);
    app.getButton()->attachLongPressStart((callbackFunction ) Wallock::callBack_buttonHold);
    app.getButton()->attachDoubleClick((callbackFunction ) Wallock::callBack_buttonDoubleClick);

    timer.setInterval( 1000, (timer_callback) Wallock::callBack_displayCurrentTime);

    Wallock::appInstance = &app;

#if ENABLE_ENCODER_RGB
    colorManager.setup();
    long colorLoop[] = { ColorNames::red, ColorNames::blue, ColorNames::green, ColorNames::purple, ColorNames::orange };
    uint8_t numColors = sizeof(colorLoop) / sizeof(long);
    for (int i = 0; i < numColors; i++) {
      colorManager.setColor(colorLoop[i]);
      delay(200);
    }
    colorManager.off();
#endif

    #if ENABLE_NEOPIXELS
        timer.setInterval( 5000, (timer_callback) Wallock::callBack_neoPixelNextEffect);
        timer.setInterval(    5, (timer_callback) Wallock::callBack_neoPixelRefresh);
    #endif

    #if ENABLE_SET_TIME
        while(!detectRTC()) {
            Serial.print(F("RTC chip not detected, can not proceed."));
            delay(1000);
        }
    #endif /* ENABLE_SET_TIME */

    #if ENABLE_NEOPIXELS
        NeoPixelEffects *n = app.neoPixelManager->effects();
        n->reset();
        for (int i = 0; i < 3 * 255; i++) {
            n->fadeCycle();
            delay(2);
        }
        n->fadeOut(2000);
        app.neoPixelManager->shutoff();
        Serial.println("Setup complete, entering loop...");
    #endif
}

void loop() {
    timer.run();
    app.run();
}
