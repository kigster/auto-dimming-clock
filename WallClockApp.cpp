/*
 * WallClockApp.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#include "WallClockApp.h"
#include <Arduino.h>


WallClockApp::WallClockApp(HardwareConfig configuration) {
    config = configuration;
    rotaryButton = new OneButton(config.pinRotaryButton, true);
    redButton = new OneButton(config.pinRedButton, true);
    matrix = new Adafruit_7segment();
    rotary = new RotaryEncoderWithButton(config.pinRotaryLeft, config.pinRotaryRight, config.pinRotaryButton);
    lcd = new LiquidCrystal_I2C(0x3F, 20, config.pinRotaryButton);
    timer = new SimpleTimer(1);
    screenOn = colonOn = true;
    mode = SetTime::Default;
    helper = new SetTimeHelper();
    brightness = 15;
    menu = NULL;
}

void WallClockApp::setup() {
    menu = new SetTimeMenu(this);
    pinMode(config.pinLED, OUTPUT);
    pinMode(config.pinPhotoResistor, INPUT);
    pinMode(config.pinRedButton, INPUT_PULLUP);
    matrix->begin(0x70);
    rotary->begin();

    lcd->init();
    // Print a message to the LCD.
    lcd->backlight();
    lcd->print("Clock-A-Roma v1.0");
    rotaryButton->attachClick(rotaryButtonClick);
    rotaryButton->attachDoubleClick(rotaryButtonDoubleClick);
    rotaryButton->attachLongPressStop(rotaryButtonHold);

    redButton->attachClick(toggleDisplay);
    timer->setInterval(1000, displayTimeNow);
    timer->setInterval(1000, readPhotoResistor);
}

void WallClockApp::updateBrightness() {
    matrix->setBrightness(brightness);
    sprintf(buf, "Brightness[0-15]:%2d", brightness);
    debug(1, buf, false);
}

void WallClockApp::loop() {
    timer->run();
    rotaryButton->tick();
    redButton->tick();

    signed int delta = rotary->rotaryDelta();
    if (abs(delta) > 1) {
        delta = (delta > 0) ? 1 : -1;
        brightness += delta;
        if (brightness < 0) brightness = 0;
        if (brightness > 15) brightness = 15;
        updateBrightness();
        delay(20);
    }
    delay(10);
}
void WallClockApp::debug(const char *message) {
    Serial.println(message);
}

void WallClockApp::debug(int row, const char *message, bool clear) {
    if (clear)
        lcd->clear();
    row = row % 4;
    lcd->setCursor(0, row);
    lcd->print(message);
    Serial.println(message);
}

void WallClockApp::blinkLED() {
    digitalWrite(config.pinLED, HIGH);
    digitalWrite(config.pinLED, LOW);
    delay(100);
    digitalWrite(config.pinLED, HIGH);
}

void WallClockApp::cb_DisplayTimeNow() {
    tmElements_t tm;
    if (RTC.read(tm)) {
        short h = tm.Hour % 12;
        if (h == 0) { h = 12; }

        short m = tm.Minute;
        if (screenOn) displayTime(h, m);
        sprintf(buf, "%2d:%02d:%02d %d/%02d/%d", h, m, tm.Second, tm.Month, tm.Day, 1970 + tm.Year);
        Serial.println(buf);
        lcd->setCursor(0,3);
        lcd->print(buf);
    } else {
        if (RTC.chipPresent()) {
            Serial.println("RTC chip found, but not initialized. Setting to compile time.");
            helper->setTimeToCompileTime();
            return;
        } else {
            matrix->printError();
            Serial.println("Time chip not detected");
            lcd->setCursor(0,1);
            lcd->println("Time chip not detected");
            colonOn = !colonOn;
        }
    }

}
/**
 * We receive negative hours or minutes when the other
 * element is being setup / modified. A bit of nasty overloading, but hey. Whatever.
 */
void WallClockApp::displayTime(signed short h, signed short m) {
    if (!screenOn) return;
    matrix->clear();
    colonOn = !colonOn;
    if (h < 0 || m < 0) colonOn = false;
    if (h > 0) {
        if (h >= 10) {
            matrix->writeDigitNum(0, h / 10, false);
        }
        matrix->writeDigitNum(1, h % 10, false);
    }
    matrix->drawColon(colonOn);
    if (m >= 0) {
        matrix->writeDigitNum(3, m / 10, false);
        matrix->writeDigitNum(4, m % 10, false);
    }
    matrix->writeDisplay();
}

void WallClockApp::cb_ReadPhotoResistor() {
    int v = analogRead(config.pinPhotoResistor);
    lcd->setCursor(0,2);
    lcd->print("Photo Value: ");
    sprintf(buf, "%4d", v);
    lcd->print(buf);
}

void WallClockApp::cb_RotaryButtonClick() {
    if (mode != SetTime::Default) {
        menu->nextMode();
    }
}

void WallClockApp::cb_RotaryButtonDoubleClick() {
    if (mode != SetTime::Default) {
        mode = SetTime::Default;
        debug(0, "Cancel Setup", true);
    }
    digitalWrite(config.pinLED, LOW);
}
void WallClockApp::cb_RotaryButtonHold() {
    Serial.println("cb_RotaryButtonHold");
    if (mode == SetTime::Default) {
        printf("Mode is default, calling configureTime() on menu pointer %x", (int) menu);
        menu->configureTime();
    }
}

void WallClockApp::cb_ToggleDisplay() {
    screenOn = !screenOn;
    matrix->clear();
    matrix->writeDisplay();
    if (screenOn) {
        displayTimeNow(0);
    }
}

