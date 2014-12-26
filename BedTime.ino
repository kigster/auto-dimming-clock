/*
 * BedTime
 *
 * 7-Segment Display for current time, based on the RTC chip.
 *
 * Rotary encoder is used to change brightness, and set time. The button
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
#include <RotaryEncoderWithButton.h>
#include <SimpleTimer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include <OneButton.h>


#include "BedTime.h"
#include "BedTimeApp.h"

HardwareConfig config = {
        A0,     // Photo
         3,     // Left Rotary
         4,     // Right Rotary
         5,     // Rotary Button
         2      // NeoPixels
};
BedTimeApp app(config);
SimpleTimer timer(1);

#ifdef TEENSYDUINO
time_t getTeensy3Time() {
    return Teensy3Clock.get();
}
#endif

void rotaryButtonClick() {
    Serial.println("rotaryButtonClick()");
    app.cb_ButtonClick();
    delay(100);
}
void rotaryButtonDoubleClick() {
    Serial.println("rotaryButtonDoubleClick()");
    app.cb_ButtonDoubleClick();
    delay(100);
}
void rotaryButtonLongPress() {
    Serial.println("rotaryButtonLongPress()");
    app.cb_ButtonHold();
    delay(100);
}
void displayTimeNow(int timerId) {
    app.displayCurrentTime();
}
void readPhotoResistor(int timerId) {
    app.getPhotoReading();
}
void neoPixelRefresh(int timerId) {
    app.neoPixelRefresh();
}
void neoPixelNextEffect(int timerId) {
    app.neoPixelNextEffect();
}

void setup() {
    Serial.begin(57600);
#ifdef TEENSYDUINO
    setSyncProvider(getTeensy3Time);
#endif
    Serial.println("Clock-A-Roma v1.0");

    Serial.println("Moving on to setup...");
    app.setup();
    displayTimeNow(0);

    Serial.println("Establishing callbacks...");
    app.rotary->getButton()->attachClick(rotaryButtonClick);
    app.rotary->getButton()->attachDoubleClick(rotaryButtonDoubleClick);
    app.rotary->getButton()->attachLongPressStart(rotaryButtonLongPress);

    timer.setInterval( 1000, displayTimeNow);
    timer.setInterval(  200, readPhotoResistor);
    timer.setInterval( 5000, neoPixelNextEffect);
    timer.setInterval(    5, neoPixelRefresh);

    Serial.print("Checking time chip and current time... ");

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
    tmElements_t tm;
    bool rtcResult = RTC.read(tm);
    if (RTC.chipPresent()) {
        if (!rtcResult || tm.Year < 2014>)) {
            Serial.print("Year returned is < 2014, resetting time.  Year: "); Serial.println(tm.Year);
            app.helper->setDateToCompileTime();
        }
    } else {
        Serial.println("RTC chip was not detected.")
    }
#endif /* TEENSYDUINO */
#endif /* ENABLE_SET_TIME */

    NeoPixelEffects *n = app.neoPixelManager->effects();
    n->reset();
    for (int i = 0; i < 3 * 255; i++) {
        n->fadeCycle();
        delay(2);
    }
    n->fadeOut(2000);
    app.neoPixelManager->shutoff();
    Serial.println("Setup complete, entering loop...");
}

void loop() {
    timer.run();
    app.rotary->tick();
    app.adjustBrightness();
    delay(10);
}
