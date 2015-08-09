/*
 * BedTimeApp.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#include <Arduino.h>
#include "../Wallock.h"
#include "App.h"

namespace Wallock {
    App::App(       PinoutMapping               &_pinout,
                    State                       &_state,
                    RotaryEncoderWithButton     &_rotary,
                    Adafruit_7segment           &_matrix) :

                    pinout(_pinout),
                    state(_state),
                    rotary(_rotary),
                    matrix(_matrix),
                    button(_rotary.button)
    {

        screenOn = colonOn = true;
        mode = SetTime::Default;
        state.getDisplayBrightness().setMinMax(0, 15);
        state.getOutsideBrightness().setMinMax(0, 1023);
        neoPixelsOn = false;
    #ifdef ENABLE_LCD
        lcd = new LiquidCrystal_I2C(0x3F, 20, 4);
    #endif
    #ifdef ENABLE_NEOPIXELS
        neoPixelManager = new NeoPixelManager(3, pinout.pinNeoPixels);
    #endif
    #ifdef ENABLE_MENU
        menu.setApp(this);
    #endif
    }

    void App::setup() {
        matrix.begin(0x70);
        matrix.clear();
        matrix.print(0x0000, HEX);
        matrix.writeDisplay();
        delay(1000);
    #ifdef ENABLE_PHOTORESISTOR
        pinMode(pinout.pinPhotoResistor, INPUT);
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

    void App::run() {
        rotary.tick();
        matrix.setBrightness(15);
        displayCurrentTime();
    //    readEnvironment();
    //    refreshUI();
    }
    void App::readEnvironment() {
        readPhotoresitor();
        readKnob();
    }

    void App::readKnob() {
        signed short delta = rotary.delta();
        if (delta != 0) {
            state.getDisplayBrightness().addDeltaToCurrent(delta);
        }
    }
    void App::readPhotoresitor() {
        uint32_t v = analogRead(pinout.pinPhotoResistor);
        state.getOutsideBrightness().setCurrent(v);

    #ifdef ENABLE_LCD
        lcd->setCursor(0,2);
        lcd->print("Photo Value: ");
        sprintf(buffer, "%4d", v);
        lcd->print(buffer);
    #endif
    }

    void App::refreshUI() {
        state.getOutsideBrightness().applyMyDeltaTo((GaugedValue *) &state.getDisplayBrightness());
        signed short currentBrightness = state.getDisplayBrightness().getCurrent();
    //    matrix.setBrightness(currentBrightness);
        sprintf(buffer, "refreshing screen brightness [0-15] to %2d", currentBrightness);
        debug(1, buffer, false);
    }

    void App::neoPixelRefresh() {
    #ifdef ENABLE_NEOPIXELS
        if (neoPixelsOn)
            neoPixelManager->refreshEffect();
    #endif
    }
    void App::neoPixelNextEffect() {
    #ifdef ENABLE_NEOPIXELS
        if (neoPixelsOn)
            neoPixelManager->nextEffect();
    #endif
    }

    void App::debug(const char *message) {
        Serial.println(message);
    }

    void App::debug(int row, const char *message, bool clear) {
    #ifdef ENABLE_LCD
        if (clear)
            lcd->clear();
        row = row % 4;
        lcd->setCursor(0, row);
        lcd->print(message);
    #endif
        Serial.println(message);
    }


    void App::displayCurrentTime() {
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
                matrix.printError();
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
    void App::displayTime(signed short h, signed short m) {
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

    void App::cb_ButtonClick() {
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

    void App::cb_ButtonDoubleClick() {
        if (mode != SetTime::Default) {
            mode = SetTime::Default;
            debug(0, "Cancel Setup", true);
        } else {
            toggleDisplay();
        }
    }

    void App::cb_ButtonHold() {
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

    void App::toggleNeoPixels() {
    #ifdef ENABLE_NEOPIXELS
        neoPixelsOn = !neoPixelsOn;
        if (neoPixelsOn) {
            neoPixelManager->nextEffect();
        } else {
            neoPixelManager->shutoff();
        }
    #endif
    }
    void App::toggleDisplay() {
        screenOn = !screenOn;
        matrix.clear();
        matrix.writeDisplay();
        if (screenOn) {
            displayCurrentTime();
        }
    }


    OneButton* App::getButton() {
        return &button;
    }
    RotaryEncoderWithButton *App::getRotary() {
        return &rotary;
    }


}
