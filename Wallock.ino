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
#include "app/State.h"

char buffer[100];

Wallock::GaugedValue gvBrightness  ("brightness", BRIGHTNESS_MIN, BRIGHTNESS_MAX, 1, true);
Wallock::GaugedValue gvPhotoReadout("photoValue", 0, 500, 40, false);

Wallock::PinoutMapping pinout = {
        A3,     // PhotoResistor
         2,     // Left Rotary
         3,     // Right Rotary
         4,     // Rotary Button
         2      // Number of NeoPixels
};

RotaryEncoderWithButton rotary(
                (uint8_t) pinout.pinRotaryLeft,
                (uint8_t) pinout.pinRotaryRight,
                (uint8_t) pinout.pinRotaryButton);
Adafruit_7segment matrix;

Wallock::State state(gvPhotoReadout, gvBrightness);

Wallock::App app(pinout, state, rotary, matrix);
SimpleTimer timer;

#ifdef ENABLE_NEOPIXELS
#include "neopixel/NeoPixelEffects.h"
#include "neopixel/NeoPixelManager.h"
#endif

#ifdef TEENSYDUINO
time_t getTeensy3Time() {
    return Teensy3Clock.get();
}
#endif

void rotaryButtonClick() {
    Serial.println(F("rotaryButtonClick()"));
    app.cb_ButtonClick();
    delay(100);
}
void rotaryButtonDoubleClick() {
    Serial.println(F("rotaryButtonDoubleClick()"));
    app.cb_ButtonDoubleClick();
    delay(100);
}
void rotaryButtonLongPress() {
    Serial.println(F("rotaryButtonLongPress()"));
    app.cb_ButtonHold();
    delay(100);
}
void displayTimeNow() {
    app.displayCurrentTime();
}
void neoPixelRefresh() {
    app.neoPixelRefresh();
}
void neoPixelNextEffect() {
    app.neoPixelNextEffect();
}

#ifdef TEENSYDUINO
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
    tmElements_t tm;
    bool rtcResult = RTC.read(tm);
    if (RTC.chipPresent()) {
        if (!rtcResult) {
            Serial.println(F("RTC.read() returned false, resetting to compile time"));
            app.helper.setDateToCompileTime();
        } else {
            sprintf(buffer, "-> %d/%d/%d %d:%d", tm.Month, tm.Day, tm.Year, tm.Hour, tm.Second);
            Serial.println(buffer);
            return true;
        }
    } else {
        Serial.println(F("RTC chip was not detected."));
    }
    return false;
}

void setup() {
    Serial.begin(9600);
#ifdef TEENSYDUINO
    setSyncProvider(getTeensy3Time);
#endif
    Serial.println(F("[wallock] v2.1(c) 2015 kiguino.moos.io"));

    app.setup();

    app.getButton()->attachClick(rotaryButtonClick);
    app.getButton()->attachLongPressStart(rotaryButtonLongPress);
    app.getButton()->attachDoubleClick(rotaryButtonDoubleClick);

    timer.setInterval( 1000, (timer_callback) displayTimeNow);
    timer.setInterval( 5000, (timer_callback) neoPixelNextEffect);
    timer.setInterval(    5, (timer_callback) neoPixelRefresh);


#ifdef ENABLE_SET_TIME
    #ifdef TEENSYDUINO
        if (year() < 2014) {
            Serial.println("[BLOOPERS!]");
            Serial.print("Year returned is < 2014, resetting time to compile time, 12:00. Year: "); Serial.println(year());
            app.helper.setDateToCompileTime();
        } else {
            Serial.println("[OK]");
        }
    #else
        while(!detectRTC()) {
            Serial.print(F("RTC chip not detected, can not proceed."));
            delay(1000);
        }
    #endif /* TEENSYDUINO */
#endif /* ENABLE_SET_TIME */

#ifdef ENABLE_NEOPIXELS
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
