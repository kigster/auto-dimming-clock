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
#include "AppInstance.h"

namespace Wallock {
    App::App(       PinoutMapping               &_pinout,
                    State                       &_state,
                    RotaryEncoderWithButton     &_rotary,
                    Adafruit_7segment           &_matrix) :

                    pinout(_pinout),
                    state (_state),
                    rotary(_rotary),
                    matrix(_matrix),
                    button(_rotary.button)
    {

        mode = SetTime::Default;
        lastDisplayedTime = 0;
        #if ENABLE_LCD
            lcd = new LiquidCrystal_I2C(0x3F, 20, 4);
        #endif
        #if ENABLE_NEOPIXELS
            neoPixelManager = new NeoPixelManager(3, pinout.pinNeoPixels);
        #endif
        #if ENABLE_MENU
            menu.setApp(this);
        #endif
    }

    void App::setup() {
        matrix.begin(0x70);
        delay(100);
        matrix.clear();
        #if ENABLE_PHOTORESISTOR
            pinMode(pinout.pinPhotoResistor, INPUT);
            delay(100);
        #endif

        #if ENABLE_LCD
            lcd->init();
            // Print a message to the LCD.
            lcd->backlight();
            debug(1, "Clock-A-Roma v1.0", true);
            debug(2, "Initialization completedl", true);
        #endif

        #if ENABLE_NEOPIXELS
            neoPixelManager->begin();
        #endif


        #if ENABLE_PHOTORESISTOR
            // set initial display brightness based on the photo
            hasPhotoReadingChanged();
            state.getBrightness().follow(&state.getPhotoReadout());
            updateDisplayBrightness();
        #endif

    }

    void App::logStatus() {
        tmElements_t tm = getCurrentTime();

        sprintf(buffer, "%2d:%02d:%02d <%4d Bytes Free> >> ",
                        tm.Hour, tm.Minute, tm.Second,
                        freeRam());
        Serial.print(buffer);
        sprintf(buffer, "Brightness: [%2d %3d%%] Sensor: [%3d %3d%%], offset [%4d]",
                        state.currentBrightness(),
                        (int)(state.getBrightness().getCurrentAsPercentOfRange()),
                        state.currentPhotoReadout(),
                        (int)(state.getPhotoReadout().getCurrentAsPercentOfRange()),
                        state.getPhotoReadout().offset);
        Serial.println(buffer);
    }


    void App::run() {
        rotary.tick();
        int brightness = state.currentBrightness();

        if (wasKnobRotated()) {
            hasPhotoReadingChanged();
            state.getPhotoReadout().syncOffsetTo(&state.getBrightness());
        } else if (hasPhotoReadingChanged()) {
            state.getBrightness().follow(&state.getPhotoReadout());
        }

        if (brightness != state.currentBrightness()) {
            updateDisplayBrightness();
            logStatus();
        }
    }

    bool App::wasKnobRotated() {
        long delta = rotary.delta();
        if (abs(delta) >= 2) {
            delta = (delta > 0) ? 1 : -1;
            if (state.getBrightness().changeCurrentBy(delta)) {
                return true;
            }
        }
        return false;
    }

    bool App::hasPhotoReadingChanged() {
        #if ENABLE_PHOTORESISTOR
            return state.getPhotoReadout().
                setCurrent(
                      analogRead(
                           pinout.pinPhotoResistor));

            #if DEBUG
                if (millis() - lastDisplayedTime > 1000) {
                    Serial.print("photoresistor readout [0-1023]: ");
                    Serial.println(state.currentPhotoReadout());
                    lastDisplayedTime = millis();
                }
            #endif

            #if ENABLE_LCD
                lcd->setCursor(0,2);
                lcd->print("Photo Value: ");
                sprintf(buffer, "%4d", state.currentPhotoReadout());
                lcd->print(buffer);
            #endif
        #else
            return false;
        #endif

    }

    bool App::is24hr() {
        return state.getValues().clock24hr;
    }

    bool App::flip24hr() {
        return state.flip24Hr();
    }

    void App::updateDisplayBrightness() {
        matrix.setBrightness(state.currentBrightness());
    }

    void App::debug(const char *message) {
        Serial.println(message);
    }

    void App::debug(int row, const char *message, bool clear) {
        #if ENABLE_LCD
            if (clear)
                lcd->clear();
            row = row % 4;
            lcd->setCursor(0, row);
            lcd->print(message);
        #endif
        Serial.println(message);
    }

    void App::displayCurrentTime() {
        tmElements_t tm = getCurrentTime();
        if (state.getValues().displayOn)
            displayTime(tm.Hour, tm.Minute);
    }
    /**
     * We receive negative hours or minutes when the other
     * element is being setup / modified. A bit of nasty overloading, but hey. Whatever.
     *
     * hours should always be passed in in a 24 hour format, ie. 0 <= h <= 23
     */
    void App::displayTime(short hour, short minute) {
        short h = hour;
        short m = minute;

        if (!state.getValues().displayOn)
            return;

        if (h < 0 && m < 0) return;

        // For dots and the colon:

        // 0x02 - left colon - lower dot
        // 0x04 - center colon
        // 0x08 - left colon - upper dot
        // 0x10 - decimal point

        matrix.clear();
        state.flipColon();
        bool colonOn = state.getValues().colonOn;

        uint8_t                bitmask  = 0x00;
        if (h < 0 || m < 0)    colonOn = false;

        if (colonOn)           { bitmask |= 0x02; }


        // hours
        if (h >= 0) {
            if ((!is24hr() || m < 0) && hour >= 12) { bitmask |= 0x08; } // pm
            if (h >= maxHour()) { h = h % maxHour(); }
            if (h == 0)         { h = maxHour(); }
            if (h >= 10)    matrix.writeDigitNum(0, h / 10, false); // leftmost number
                            matrix.writeDigitNum(1, h % 10, false);
        }
        // minutes
        if (m >= 0) {
            m = m % 60;
                            matrix.writeDigitNum(3, m / 10, false);
                            matrix.writeDigitNum(4, m % 10, false);
        }
                            matrix.writeDigitRaw(2, bitmask); // dots

        matrix.writeDisplay();
    }


    tmElements_t App::getCurrentTime() {
        tmElements_t tm;

        #if TEENSYDUINO
            breakTime(now(), tm);
        #else
            if (!RTC.read(tm)) {
                if (RTC.chipPresent()) {
                    Serial.println(F("Time chip detected, but not set. Resetting"));
                    #if ENABLE_SET_TIME
                        helper.setDateToCompileTime();
        #endif
                } else {
                    matrix.printError();
                    Serial.println(F("Time chip not detected"));
                    state.flipColon();
                }
            }
        #endif
        return tm;
    }

    int App::maxHour() {
        return is24hr() ? 24 : 12;
    }

    void App::eventClick() {
        delay(100);
        Serial.print(F("Entering BedTimeApp::cb_ButtonClick, mode = "));
        Serial.println((int) mode);
        if (mode != SetTime::Default) {
            #if ENABLE_MENU
                menu.nextMode();
            #endif
        } else {
            state.flip24Hr();
            #if ENABLE_NEOPIXELS
                toggleNeoPixels();
            #endif
        }
    }

    void App::eventDblClick() {
        delay(100);
        if (mode != SetTime::Default) {
            mode = SetTime::Default;
            debug(0, "Cancel Setup", true);
        } else {
            toggleDisplay();
        }
    }

    void App::eventHold() {
        Serial.print(F("Entering BedTimeApp::cb_ButtonHold, mode = "));
        Serial.println((int) mode);
        if (mode == SetTime::Default) {
            Serial.println(F("Mode is Default -> calling configureTime()"));
            #if ENABLE_MENU
                delay(200);
                menu.configureTime();
            #endif
        } else {
            Serial.println(F("Mode is not Default, Hold is ignored."));
        }
    }
    void App::toggleDisplay() {
        state.flipDisplayOn();
        matrix.clear();
        matrix.writeDisplay();
        if (state.getValues().displayOn) {
            displayCurrentTime();
        }
    }


    void App::neoPixelRefresh() {
        #if ENABLE_NEOPIXELS
            if (neoPixelsOn)
                neoPixelManager->refreshEffect();
        #endif
    }
    void App::neoPixelNextEffect() {
        #if ENABLE_NEOPIXELS
            if (neoPixelsOn)
                neoPixelManager->nextEffect();
        #endif
    }

    void App::toggleNeoPixels() {
         #if ENABLE_NEOPIXELS
            neoPixelsOn = !neoPixelsOn;
            if (neoPixelsOn) {
                neoPixelManager->nextEffect();
            } else {
                neoPixelManager->shutoff();
            }
        #endif
    }

    OneButton* App::getButton() {
        return &button;
    }
    RotaryEncoderWithButton *App::getRotary() {
        return &rotary;
    }

}
