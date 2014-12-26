/*
 * BedTimeApp.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#include "BedTimeApp.h"
#include <Arduino.h>

BedTimeApp::BedTimeApp(HardwareConfig configuration) {
    config = configuration;
#ifdef ENABLE_LCD
    lcd = new LiquidCrystal_I2C(0x3F, 20, config.pinRotaryButton);
#endif
    screenOn = colonOn = true;
    mode = SetTime::Default;
    neoPixelManager = new NeoPixelManager(3, config.pinNeoPixels);
    rotary = new RotaryEncoderWithButton(config.pinRotaryLeft, config.pinRotaryRight, config.pinRotaryButton);
    brightness = 15;
    neoPixelsOn = false;
    lastPhotoReading = 0;
#ifdef ENABLE_MENU
    menu = NULL;
#endif
}

void BedTimeApp::setup() {
    matrix.begin(0x70);
    matrix.clear();
    matrix.writeDisplay();

#ifdef ENABLE_MENU
    if (!menu)
        menu = new SetTimeMenu(this);
#endif
    pinMode(config.pinPhotoResistor, INPUT);

#ifdef ENABLE_LCD
    lcd->init();
    // Print a message to the LCD.
    lcd->backlight();
    lcd->print("Clock-A-Roma v1.0");
#endif

    neoPixelManager->begin();
}

uint8_t BedTimeApp::getBrightness() {
    return brightness;
}

void BedTimeApp::adjustBrightness() {
    signed short delta = rotary->delta();
    if (delta != 0) {
        setBrightness((signed short) getBrightness() + delta);
    }
}

void BedTimeApp::setBrightness(signed short brightnessValue) {
    if (brightnessValue > 15) brightnessValue = 15;
    if (brightnessValue < 0) brightnessValue = 0;
    brightness = brightnessValue;
    matrix.setBrightness(brightness);
    sprintf(buffer, "Brightness[0-15]:%2d", brightness);
    debug(1, buffer, false);
}

void BedTimeApp::neoPixelRefresh() {
    if (neoPixelsOn)
        neoPixelManager->refreshEffect();
}
void BedTimeApp::neoPixelNextEffect() {
    if (neoPixelsOn)
        neoPixelManager->nextEffect();
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
            Serial.println("RTC chip found, but not initialized. Setting to compile time.");
            helper.setTimeToCompileTime();
            return;
        } else {
            matrix.printError();
            Serial.println("Time chip not detected");
            lcd->setCursor(0,1);
            lcd->println("Time chip not detected");
            colonOn = !colonOn;
            return;
        }
    }
#endif
    h = tm.Hour % 12;
    if (h == 0) { h = 12; }
    m = tm.Minute;
    if (screenOn) displayTime(h, m);
    sprintf(buffer, "%2d:%02d:%02d %d/%02d/%d", h, m, tm.Second, tm.Month, tm.Day, 1970 + tm.Year);
    Serial.println(buffer);
#ifdef ENABLE_LCD
    lcd->setCursor(0,3);
    lcd->print(buffer);
#endif
}
/**
 * We receive negative hours or minutes when the other
 * element is being setup / modified. A bit of nasty overloading, but hey. Whatever.
 */
void BedTimeApp::displayTime(signed short h, signed short m) {
    if (!screenOn && h >= 0 && m >= 0) return;
    matrix.clear();
    colonOn = !colonOn;
    if (h < 0 || m < 0) colonOn = false;
    if (h > 0) {
        if (h >= 10) {
            matrix.writeDigitNum(0, h / 10, false);
        }
        matrix.writeDigitNum(1, h % 10, false);
    }
    matrix.drawColon(colonOn);
    if (m >= 0) {
        matrix.writeDigitNum(3, m / 10, false);
        matrix.writeDigitNum(4, m % 10, false);
    }
    matrix.writeDisplay();
}

void BedTimeApp::getPhotoReading() {

    uint16_t v = analogRead(config.pinPhotoResistor);
    if (lastPhotoReading == 0) {
        lastPhotoReading = v;
        return;
    }

    signed long delta = 8 * (v - lastPhotoReading) / 1024; // convert to brightness
    if (delta != 0) {
        Serial.print("PhotoResistor reading is: ");
        Serial.print(v);
        Serial.print(". Changing brightness from "); Serial.print(brightness);
        Serial.print(" to "); Serial.println(delta);
        setBrightness(brightness + delta);
    }

    lastPhotoReading = v;
#ifdef ENABLE_LCD
    lcd->setCursor(0,2);
    lcd->print("Photo Value: ");
    sprintf(buffer, "%4d", v);
    lcd->print(buffer);
#endif
}

void BedTimeApp::cb_ButtonClick() {
    Serial.print("Entering BedTimeApp::cb_ButtonClick, mode = ");
    Serial.println((int) mode);
    if (mode != SetTime::Default) {
#ifdef ENABLE_MENU
        menu->nextMode();
#endif
    } else {
        toggleNeoPixels();
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
        if (menu)
            menu->configureTime();
        else
            Serial.println("Whoops, menu point is NULL!!");
    } else {
        Serial.println("Mode is not Default, Hold is ignored.");
    }
}

void BedTimeApp::toggleNeoPixels() {
    neoPixelsOn = !neoPixelsOn;
    if (neoPixelsOn) {
        neoPixelManager->nextEffect();
    } else {
        neoPixelManager->shutoff();
    }
}
void BedTimeApp::toggleDisplay() {
    screenOn = !screenOn;
    matrix.clear();
    matrix.writeDisplay();
    if (screenOn) {
        displayCurrentTime();
    }
}

