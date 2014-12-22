/*
 * WallClock
 *
 * 7-Segment Display for current time, based on the RTC chip.
 *
 * Rotary encoder is used to set the time.
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
#include "printf.h"
#include "WallClock.h"
#include "WallClockApp.h"
#define br     Serial.println("")

HardwareConfig config = {
        13,     // LED
        A0,     // Photo
         5,     // Button
         3,     // Right Rotary
         2,     // Left Rotary
         4 };   // Click Rotary

WallClockApp *app;

void rotaryButtonClick() {
    app->cb_RotaryButtonClick();
}
void rotaryButtonDoubleClick() {
    app->cb_RotaryButtonDoubleClick();
}
void rotaryButtonHold() {
    Serial.println("rotaryButtonHold()");
    app->cb_RotaryButtonHold();
}
void toggleDisplay() {
    app->cb_ToggleDisplay();
}
void displayTimeNow(int timerId) {
    app->cb_DisplayTimeNow();
}
void readPhotoResistor(int timerId) {
    app->cb_ReadPhotoResistor();
}

void setup() {
    Serial.begin(57600);
    printf_begin();

    printf("Creating Application instance...\n"); br;

    app = new WallClockApp(config);
    printf("Calling setup...\n"); br;

    app->setup();
    printf("Setup complete, entering loop.\n"); br;
}

void loop() {
    app->loop();
}
