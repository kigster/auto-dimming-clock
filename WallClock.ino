/*
 * TwoLeds – this example demonstrates using RotaryEncoder with a button.
 *
 * Dependencies for the library:
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * Dependencies for the example:
 *
 * RotaryEncoderWithButton library
 * SimpleTimer library
 *
 *  Created on: Aug 9, 2014
 *      Author: Konstantin Gredeskoul
 *
 *  (c) 2014 All rights reserved.  Please see LICENSE.
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
#include "CompileTimeManager.h"

OneButton                   button(4, true);
Adafruit_7segment           matrix = Adafruit_7segment();
RotaryEncoderWithButton     rotary(2,3,4);
LiquidCrystal_I2C           lcd(0x3F,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
SimpleTimer                 timer(1);

uint8_t                     pinLED = 13;
bool                        colonOn = false;
char                        buf[30];

void updateBrigthness() {
    puts(0, "==== ==== ==== ==== ", true);
    matrix.setBrightness(brightness);
    sprintf(buf, "Brightness[0-15]:%2d", brightness);
    puts(1, buf, false);
    puts(2, "==== ==== ==== ==== ", false);
}


//_________________________________________________________________________________________

void setup() {
    Serial.begin(57600);
    pinMode(pinLED, OUTPUT);
    matrix.begin(0x70);
    rotary.begin();

// #define SET_TO_COMPILE_TIME
#ifdef SET_TO_COMPILE_TIME
    CompileTimeManager *timeManager = new CompileTimeManager(saveNewTime);
    timeManager->setTimeToCompileTime();
#endif

    lcd.init();
    // Print a message to the LCD.
    lcd.backlight();
    button.attachClick(buttonClick);
    button.attachDoubleClick(buttonDoubleClick);
    button.attachLongPressStop(buttonHold);
    timer.setInterval(1000, displayTimeNow);
}

void buttonClick() {
    if (mode != SetTime::Default) {
        nextMode();
    }
}

void buttonDoubleClick() {
    if (mode != SetTime::Default) {
        mode = SetTime::Default;
        puts(0, "Cancel Setup", true);
    }
}
void buttonHold() {
    if (mode == SetTime::Default)
        configureTime();
}

void loop() {
    timer.run();
    button.tick();

    signed int delta = rotary.rotaryDelta();
    if (abs(delta) > 1) {
        delta = (delta > 0) ? 1 : -1;
        brightness += delta;
        if (brightness < 0) brightness = 0;
        if (brightness > 15) brightness = 15;
        updateBrigthness();
        delay(20);
    }
    delay(10);
}
