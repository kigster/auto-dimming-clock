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
                    state (_state),
                    rotary(_rotary),
                    matrix(_matrix),
                    button(_rotary.button)
    {

        screenOn = colonOn = true;
        mode = SetTime::Default;
        photoOffsetPercentage = lastPhotoValue = currentPhotoValue = lastDisplayedTime = 0;
        neoPixelsOn = false;
        clock24hr = true;
        state.getDisplayBrightness().setCurrent(7);
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
        delay(100);
        matrix.clear();
        changeBrightness();
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

    bool App::is24hr() {
        return clock24hr;
    }

    void App::run() {
        rotary.tick();
        if (processKnobEvents() || processPhotoresistorChange()) {
            Serial.println();
        }
    }

    bool App::processKnobEvents() {
        long delta = rotary.delta();
        if (abs(delta) >= 2) {
            delta = (delta > 0) ? 1 : -1;
            if (state.getDisplayBrightness().changeBy(delta)) {
                changeBrightness();
//                state.getPhotoresistorReading().syncTo(&state.getDisplayBrightness());
                return true;
            }
        }
        return false;
    }

    bool App::processPhotoresistorChange() {
        static GaugedValue &photo = state.getPhotoresistorReading();
        static GaugedValue &display = state.getDisplayBrightness();

        currentPhotoValue = analogRead(pinout.pinPhotoResistor);

        #ifdef DEBUG
            if (millis() - lastDisplayedTime > 1000) {
                Serial.print("photoresistor readout [0-1023]: ");
                Serial.println(currentPhotoValue);
                lastDisplayedTime = millis();
            }
        #endif

        if (currentPhotoValue != lastPhotoValue) {
            if (photo.setCurrent(currentPhotoValue) ) {
                lastPhotoValue = currentPhotoValue;
//                if (display.follow(&photo)) {
//                    changeBrightness();
//                    return true;
//                }
            }
        }

        #ifdef ENABLE_LCD
            lcd->setCursor(0,2);
            lcd->print("Photo Value: ");
            sprintf(buffer, "%4d", v);
            lcd->print(buffer);
        #endif

        return false;
    }

    void App::changeBrightness() {
        matrix.setBrightness(state.getDisplayBrightness().getCurrent());
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
                    Serial.println(F("Time chip detected, but not set. Resetting"));
                    #ifdef ENABLE_SET_TIME
                        helper.setDateToCompileTime();
                    #endif
                } else {
                    matrix.printError();
                    Serial.println(F("Time chip not detected"));
                    colonOn = !colonOn;
                }
                return;
            }
        #endif

        if (screenOn) displayTime(tm.Hour, tm.Minute);
        Serial.print(F("> "));
        sprintf(buffer, "%2d:%02d:%02d %d/%02d/%d, BR: [%d] PH: [%d] FreeMem: [%d]",
                        h, m, tm.Second, tm.Month, tm.Day, 1970 + tm.Year,
                        state.getDisplayBrightness().getCurrent(),
                        state.getPhotoresistorReading().getCurrent(),
                        freeRam());
        debug(buffer);
    }

    int App::maxHour() {
        return is24hr() ? 24 : 12;
    }
    /**
     * We receive negative hours or minutes when the other
     * element is being setup / modified. A bit of nasty overloading, but hey. Whatever.
     */
    void App::displayTime(short h, short m) {
        if (!screenOn && h >= 0 && m >= 0) return;

        short hour = h; short minute = m;

        // For dots and the colon:

        // 0x02 - left colon - lower dot
        // 0x04 - center colon
        // 0x08 - left colon - upper dot
        // 0x10 - decimal point
        long now = millis();
        matrix.clear();
        colonOn = !colonOn;
        uint8_t                      bitmask  = 0x00;
        if (h < 0 || m < 0)          colonOn = false;
        if (h >= 0) {
            h = h % maxHour();
            if (h == 0) { h = maxHour(); }

            if (colonOn)               { bitmask |= 0x02; }
            if (!is24hr() && h >= 12)  { bitmask |= 0x08; } // pm

            if (h >= 10)    matrix.writeDigitNum(0, h / 10, false); // leftmost number
                            matrix.writeDigitNum(1, h % 10, false);
        }
        if (m >= 0) {
                            matrix.writeDigitNum(3, m / 10, false);
                            matrix.writeDigitNum(4, m % 10, false);
        }
                            matrix.writeDigitRaw(2, bitmask); // dots

        matrix.writeDisplay();
    }

    void App::cb_ButtonClick() {
        Serial.print(F("Entering BedTimeApp::cb_ButtonClick, mode = "));
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
        Serial.print(F("Entering BedTimeApp::cb_ButtonHold, mode = "));
        Serial.println((int) mode);
        if (mode == SetTime::Default) {
            Serial.println(F("Mode is Default -> calling configureTime()"));
    #ifdef ENABLE_MENU
//            timer.disable(0);
//            timer.disable(1);
            menu.configureTime();
    #endif
        } else {
            Serial.println(F("Mode is not Default, Hold is ignored."));
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
