#include <TimeLib.h>

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
#include "Wallock.h"
#include "App.h"
#include "AppInstance.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

extern uint32_t freeRam();

namespace Wallock {
#if ENABLE_ENCODER_RGB
    App::App(       PinoutMapping               &_pinout,
                    State                       &_state,
                    RotaryEncoderWithButton     &_rotary,
                    OneButton                   &_button,
                    Adafruit_7segment           &_matrix,
                    RGBController               &_colorManager) :
                    pinout(_pinout),
                    state (_state),
                    rotary(_rotary),
                    button(_button),
                    matrix(_matrix),
                    rgbController(_colorManager)
#else
    App::App(       PinoutMapping               &_pinout,
                    State                       &_state,
                    RotaryEncoderWithButton     &_rotary,
                    OneButton                   &_button,
                    Adafruit_7segment           &_matrix) :
                    pinout(_pinout),
                    state (_state),
                    rotary(_rotary),
                    button(_button),
                    matrix(_matrix)
#endif
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
        #if ENABLE_ENCODER_RGB
        patternManager = new RGBPatternManager(rgbController);
        #endif
    }

    void App::setup() {
        randomSeed(analogRead(0));
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
            readPhotoTrueIfChanged();
            state.getBrightness().follow(&state.getPhotoReadout());
            updateDisplayBrightness();
        #endif

    }

    void App::logStatus() {
        int now = millis();

        if (now - lastLoggedAt > 1000) { 
          tmElements_t tm = getCurrentTime();
          lastLoggedAt = now;
 
          sprintf(buffer, "%2d/%2d/%4d %2d:%02d:%02d | Free Ram: %4dB | ",
                          tm.Month, tm.Day, tmYearToCalendar(tm.Year),
                          tm.Hour, tm.Minute, tm.Second,
                          freeRam());
          Serial.print(buffer);
          sprintf(buffer, "Brightness: %2d ~%3d%% | Range: [%d..%d] | Photo: %3d ~%3d%% | Pot: %d ~%3d%",
                          state.currentBrightness(),
                          (int)(state.getBrightness().getCurrentAsPercentOfRange()),
                          (int)(state.getBrightness().getRangeStart()),
                          (int)(state.getBrightness().getRangeEnd()),
                          state.currentPhotoReadout(),
                          (int)(state.getPhotoReadout().getCurrentAsPercentOfRange()),
                          lastPotValue, 
                          100.0 * lastPotValue / 1023.0
                          );
          Serial.println(buffer);
        }
    }


    void App::run() {
        #if ENABLE_ENCODER_RGB
        rgbController.tick();
        #endif 

        getButton()->tick();
        
        int brightness = state.currentBrightness();

        if (wasKnobRotated()) {
            state.getPhotoReadout().syncOffsetTo(&state.getBrightness());
        } else if (readPotentiometer()) {          
            updateDisplayBrightness();         
        } else if (readPhotoTrueIfChanged()) {         
            state.getBrightness().follow(&state.getPhotoReadout());
            updateDisplayBrightness();
        } 
        logStatus();
    }

    bool App::readPotentiometer() {
      int now = millis();
      if (now - potCheckedAt > 40) {
        potCheckedAt = now;
        int val = analogRead(pinout.pinPot);    // read the value from the sensor
        if (abs(val - lastPotValue) > 1) { 
          sprintf(buffer, "Potentiometer Readout: %5d", val);
          Serial.println(buffer);
          state.getBrightness().setCurrentFromPercentage(100.0 * MIN(val, 1000) / 1000);
          lastPotValue = val; 
          
          val = val - 512;
          
          if (val < 100 && val > -100) {
            state.getBrightness().setMax(BRIGHTNESS_MAX);
            state.getBrightness().setMin(BRIGHTNESS_MIN);
          } else if (val == 1023 - 512) {
            state.getBrightness().setMax(BRIGHTNESS_MAX);
            state.getBrightness().setMin(BRIGHTNESS_MIN);
            state.getBrightness().setCurrentFromPercentage(100.0);
          } else if (val > 0) {
            int min = map(val, 0, 512, BRIGHTNESS_MIN, BRIGHTNESS_MAX/2);
            state.getBrightness().setMax(BRIGHTNESS_MAX);
            state.getBrightness().setMin(min);
          } else if (val < 0) {
            int max = map(val, -512, 0, BRIGHTNESS_MAX/2, BRIGHTNESS_MAX);
            state.getBrightness().setMax(max);
            state.getBrightness().setMin(BRIGHTNESS_MIN);
          }
          
          return true;
        }
      }             
      return false;
    }

    bool App::wasKnobRotated() {
        long delta = rotary.rotaryDelta();
        return false;
    }

    bool App::readPhotoTrueIfChanged() {
#if ENABLE_PHOTORESISTOR
        int now = millis();
        if (now - lastPhotoCheckedAt > 200) {        
            lastPhotoCheckedAt = now;
            int reading = analogRead(pinout.pinPhotoResistor);
            bool changed = state.getPhotoReadout().setCurrent(reading);

#if DEBUG
            if (changed || now - lastDisplayedTime > 1000) {
                sprintf(buffer, "Photo Resistor Raw: (%3d) {0..1023} | ", reading);
                Serial.println(buffer);
                logStatus();
                lastDisplayedTime = now;
            }
#endif

#if ENABLE_LCD
            lcd->setCursor(0,2);
            lcd->print("Photo Value: ");
            sprintf(buffer, "%4d", state.currentPhotoReadout());
            lcd->print(buffer);
#endif

            return changed;
        } else {
          return false;
        }
        #else
        return false;
        #endif
    }

    void App::showColor(long color) {
#if ENABLE_ENCODER_RGB
      rgbController.blink(color, 1010, 1);
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
        if (h < 0 && m < 0) return;

        // For dots and the colon:
        // 0x02 - left colon - lower dot
        // 0x04 - center colon
        // 0x08 - left colon - upper dot
        // 0x10 - decimal point

        matrix.clear();
        bool colonOn = state.getValues().colonOn;
        uint8_t bitmask  = 0x00;
        if (h < 0 || m < 0) {
          // configuring time
          colonOn = false;
        }
        else {
          // showing time
          if (!state.getValues().displayOn)
              return;
          state.flipColon();
          if (colonOn) {
            bitmask |= 0x02;
          }
          #if ENABLE_ENCODER_RGB
          patternManager->fadeToNext(500);
          #endif
        }

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
                        helper.setTimeToCompileTime();
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
        Serial.print(F("Entering BedTimeApp::cb_ButtonHold, mode = "));
        Serial.println((int) mode);
        if (mode == SetTime::Default) {
           Serial.println(F("Mode is Default -> calling configureTime()"));
           #if ENABLE_MENU
               delay(200);
               menu.configureTime();
               delay(200);
           #endif
        }
    }

    void App::eventHold() {
        delay(200);
#if ENABLE_ENCODER_RGB
        if (rgbController.isEnabled()) { rgbController.disable(); } 
#endif        
        toggleDisplay();

#if ENABLE_ENCODER_RGB
        if (state.getValues().displayOn) { rgbController.enable(); }
#endif        
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
