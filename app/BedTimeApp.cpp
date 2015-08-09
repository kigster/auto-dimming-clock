/*
 * BedTimeApp.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#include "../BedTime.h"
#include "BedTimeApp.h"
#include <Arduino.h>

BedTimeApp::BedTimeApp(HardwareConfig *_config) {
//    memset(&lastDisplayedTime, 0x0, sizeof(tmElements_t));
    config = _config;
    state = config->state;
#ifdef ENABLE_LCD
    lcd = new LiquidCrystal_I2C(0x3F, 20, 4);
#endif
    screenOn = colonOn = true;
    mode = SetTime::Default;
#ifdef ENABLE_NEOPIXELS
    neoPixelManager = new NeoPixelManager(3, config->pinNeoPixels);
#endif
    rotary = new RotaryEncoderWithButton(config->pinRotaryLeft, config->pinRotaryRight, config->pinRotaryButton);
    matrix = new Adafruit_7segment;
    state->getDisplayBrightness()->setMinMax(0, 15);
    state->getOutsideBrightness()->setMinMax(0, 1023);
    neoPixelsOn = false;
#ifdef ENABLE_MENU
    menu.setApp(this);
#endif
}

void BedTimeApp::setup() {
    matrix->begin(0x70);
    matrix->clear();
    matrix->print(0x0000, HEX);
    matrix->writeDisplay();
    delay(1000);
#ifdef ENABLE_PHOTORESISTOR
    pinMode(config->pinPhotoResistor, INPUT);
#endif

#ifdef ENABLE_LCD
    lcd->init();
    // Print a message to the LCD.
    lcd->backlight();
    debug(1, "Clock-A-Roma v1.0", true);
    debug(2, "Initialization completedl", true);
#endif

#ifdef ENABLE_NEOPIXELS
    neoPixelManager->begin();
#endif
}
void BedTimeApp::run() {
    rotary->tick();
    matrix->setBrightness(15);
    displayCurrentTime();
//    readEnvironment();
//    refreshUI();
}
void BedTimeApp::readEnvironment() {
    readPhotoresitor();
    readKnob();
}

void BedTimeApp::readKnob() {
    signed short delta = rotary->delta();
    if (delta != 0) {
        state->getDisplayBrightness()->addDeltaToCurrent(delta);
    }
}
void BedTimeApp::readPhotoresitor() {
    uint32_t v = analogRead(config->pinPhotoResistor);
    state->getOutsideBrightness()->setCurrent(v);

#ifdef ENABLE_LCD
    lcd->setCursor(0,2);
    lcd->print("Photo Value: ");
    sprintf(buffer, "%4d", v);
    lcd->print(buffer);
#endif
}

void BedTimeApp::refreshUI() {
    state->getOutsideBrightness()->applyMyDeltaTo(state->getDisplayBrightness());
    signed short currentBrightness = state->getDisplayBrightness()->getCurrent();
//    matrix->setBrightness(currentBrightness);
    sprintf(buffer, "refreshing screen brightness [0-15] to %2d", currentBrightness);
    debug(1, buffer, false);
}

void BedTimeApp::neoPixelRefresh() {
#ifdef ENABLE_NEOPIXELS
    if (neoPixelsOn)
        neoPixelManager->refreshEffect();
#endif
}
void BedTimeApp::neoPixelNextEffect() {
#ifdef ENABLE_NEOPIXELS
    if (neoPixelsOn)
        neoPixelManager->nextEffect();
#endif
}

void BedTimeApp::debug(const char *message) {
    Serial.println(message);
}

void BedTimeApp::debug(int row, const char *message, bool clear) {
#ifdef ENABLE_LCD
    if (clear)
        lcd->clear();
    row = row % 4;
    lcd->setCursor(0, row);
    lcd->print(message);
#endif
    Serial.println(message);
}


void BedTimeApp::displayCurrentTime() {
    tmElements_t tm;
    short h, m;
#ifdef TEENSYDUINO
    breakTime(now(), tm);
#else
    if (!RTC.read(tm)) {
        if (RTC.chipPresent()) {
            debug(1, "Time chip detected, but not set. Resetting", true);
#ifdef ENABLE_SET_TIME
            helper.setDateToCompileTime();
#endif
        } else {
            matrix->printError();
            debug(1, "Time chip not detected", true);
            colonOn = !colonOn;
        }
        return;
    }
#endif
    h = tm.Hour % 12;
    if (h == 0) { h = 12; }
    m = tm.Minute;
    if (screenOn) displayTime(h, m);
    sprintf(buffer, "%2d:%02d:%02d %d/%02d/%d", h, m, tm.Second, tm.Month, tm.Day, 1970 + tm.Year);
    debug(2, buffer, true);
}
/**
 * We receive negative hours or minutes when the other
 * element is being setup / modified. A bit of nasty overloading, but hey. Whatever.
 */
void BedTimeApp::displayTime(signed short h, signed short m) {
    if (!screenOn && h >= 0 && m >= 0) return;
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

void BedTimeApp::cb_ButtonClick() {
    Serial.print("Entering BedTimeApp::cb_ButtonClick, mode = ");
    Serial.println((int) mode);
    if (mode != SetTime::Default) {
#ifdef ENABLE_MENU
        menu.nextMode();
#endif
    } else {
#ifdef ENABLE_NEOPIXELS
        toggleNeoPixels();
#endif
    }
}

void BedTimeApp::cb_ButtonDoubleClick() {
    if (mode != SetTime::Default) {
        mode = SetTime::Default;
        debug(0, "Cancel Setup", true);
    } else {
        toggleDisplay();
    }
}

void BedTimeApp::cb_ButtonHold() {
    Serial.print("Entering BedTimeApp::cb_ButtonHold, mode = ");
    Serial.println((int) mode);
    if (mode == SetTime::Default) {
        Serial.println("Mode is Default -> calling configureTime()");
#ifdef ENABLE_MENU
        menu.configureTime();
#endif
    } else {
        Serial.println("Mode is not Default, Hold is ignored.");
    }
}

void BedTimeApp::toggleNeoPixels() {
#ifdef ENABLE_NEOPIXELS
    neoPixelsOn = !neoPixelsOn;
    if (neoPixelsOn) {
        neoPixelManager->nextEffect();
    } else {
        neoPixelManager->shutoff();
    }
#endif
}
void BedTimeApp::toggleDisplay() {
    screenOn = !screenOn;
    matrix->clear();
    matrix->writeDisplay();
    if (screenOn) {
        displayCurrentTime();
    }
}
