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
        brightnessChangedEvent();
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
        processKnobEvents();
        processPhotoresistorChange();
    }

    bool App::processKnobEvents() {
        signed long delta = rotary.delta();
        if (delta != 0) {
            // normalize knob to increments of 1
            delta = delta / abs(delta);
            if (state.getDisplayBrightness().changeBy(delta)) {
                brightnessChangedEvent();
                photoOffsetPercentage = state.getDisplayBrightness().getLastChangeAsPercentOfRange();
                return true;
            }
        }
        return false;
    }

    bool App::processPhotoresistorChange() {
        GaugedValue &photo = state.getPhotoresistorReading();
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
                GaugedValue &display = state.getDisplayBrightness();
                if (display.follow(&photo, photoOffsetPercentage)) {
                    brightnessChangedEvent();
                    return true;
                }
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

    void App::brightnessChangedEvent() {
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
        Serial.print(F("> "));
        sprintf(buffer, "%2d:%02d:%02d %d/%02d/%d, Brightness [%d], Photo [%d]",
                        h, m, tm.Second, tm.Month, tm.Day, 1970 + tm.Year,
                        state.getDisplayBrightness().getCurrent(),
                        state.getPhotoresistorReading().getCurrent());
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
