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

#include "WallClock.h"
#include "WallClockApp.h"

HardwareConfig config = {13, A0, 5, 3, 2, 4 };

WallClockApp app(config);

void rotaryButtonClick() {
    app.cb_RotaryButtonClick();
}
void rotaryButtonDoubleClick() {
    app.cb_RotaryButtonDoubleClick();
}
void rotaryButtonHold() {
    app.cb_RotaryButtonHold();
}
void toggleDisplay() {
    app.cb_ToggleDisplay();
}
void displayTimeNow(int timerId) {
    app.cb_DisplayTimeNow();
}
void readPhotoResistor(int timerId) {
    app.cb_ReadPhotoResistor();
}




void setup() {
    app.setup();
}

void loop() {
    app.loop();
}
