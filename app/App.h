/*
 * BedTimeApp.h
 *
 *  Created on: Dec 19, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#ifndef BEDTIMEAPP_H_
#define BEDTIMEAPP_H_

#include "../Wallock.h"
#include "State.h"



#if ENABLE_LCD
#include <LiquidCrystal_I2C.h>
#endif

#if ENABLE_SET_TIME
#include "../settingtime/SetTimeHelper.h"
#include "../settingtime/SetTimeMenu.h"
#endif

#if ENABLE_NEOPIXELS
#include "../neopixel/NeoPixelManager.h"
#include "../neopixel/NeoPixelEffects.h"
#endif

namespace SetTime {
    typedef enum TimeChangeMode_e {
        Default = (1 << 0), Hour = (1 << 1), Minute = (1 << 2), Save = (1 << 4), Last = (1 << 5),
    } TimeMode;
};


namespace Wallock {

    typedef struct PinoutMapping_s {
         uint8_t pinPhotoResistor;
         uint8_t pinRotaryLeft;
         uint8_t pinRotaryRight;
         uint8_t pinRotaryButton;
         uint8_t pinNeoPixels;
     } PinoutMapping;


     class App {
        private:
            PinoutMapping                               &pinout;
            State                                       &state;
            RotaryEncoderWithButton                     &rotary;
            Adafruit_7segment                           &matrix;
            OneButton                                   &button;

            uint32_t                                    lastDisplayedTime;
            void logStatus();

            bool hasPhotoReadingChanged();
            bool wasKnobRotated();

        public:
            App(            PinoutMapping               &_pinout,
                            State                       &_state,
                            RotaryEncoderWithButton     &_rotary,
                            Adafruit_7segment           &_matrix);

            SetTime::TimeMode mode;

            #if ENABLE_LCD
                LiquidCrystal_I2C *lcd;  // set the LCD address to 0x27 for a 16 chars and 2 line display
            #endif
            #if ENABLE_MENU
                SetTimeMenu menu;
            #endif
            #if ENABLE_SET_TIME
                SetTimeHelper helper;
            #endif
            #if ENABLE_NEOPIXELS
                NeoPixelManager *neoPixelManager;
            #endif

            void setup();
            void run();

            bool is24hr();
            int  maxHour();
            bool flip24hr();
            void displayCurrentTime();
            tmElements_t getCurrentTime();
            void displayTime(short h, short m);

            void updateDisplayBrightness();

            void toggleDisplay();
            void toggleNeoPixels();
            void neoPixelRefresh();
            void neoPixelNextEffect();

            void eventClick();
            void eventDblClick();
            void eventHold();

            void debug(int row, const char *message, bool clear);
            void debug(const char *message);


            OneButton* getButton();
            RotaryEncoderWithButton *getRotary();
     };
}

#endif /* BEDTIMEAPP_H_ */
