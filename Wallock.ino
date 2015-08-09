/*
 * BedTime
 *
 * 7-Segment Display for current time, based on the RTC chip.
 *
 * Rotary encoder is used to change s, and set time. The button
 * on rotary encoder is used in single click, double click and hold capacity
 * for various options. See README for more info.
 *
 * Press and hold to enter configuration menu, then click to advance
 * to the next.
 *
 * (c) 2014 All rights reserved.  Please see LICENSE.
 *
 * Author: Konstantin Gredeskoul <kigster@gmail.com>
 */
#include "Wallock.h"

#ifdef ENABLE_NEOPIXELS
#include "neopixel/NeoPixelEffects.h"
#include "neopixel/NeoPixelManager.h"
#endif

#include "app/App.h"
#include "app/State.h"

State state(new GaugedValue("display", 0, 15),
            new GaugedValue("photoresistor", (1 << 0), (1 << 10)));

HardwareConfig config = {
        A3,     // PhotoResistor
         2,     // Left Rotary
         3,     // Right Rotary
         4,     // Rotary Button
         2,     // NeoPixels
         &state
};

char buffer[128];
App app(&config);
SimpleTimer timer(1);

#ifdef TEENSYDUINO
time_t getTeensy3Time() {
    return Teensy3Clock.get();
}
#endif

void rotaryButtonClick() {
    Serial.println("rotaryButtonClick()");
    app.cb_ButtonClick();
    delay(100);
}
void rotaryButtonDoubleClick() {
    Serial.println("rotaryButtonDoubleClick()");
    app.cb_ButtonDoubleClick();
    delay(100);
}
void rotaryButtonLongPress() {
    Serial.println("rotaryButtonLongPress()");
    app.cb_ButtonHold();
    delay(100);
}
void displayTimeNow(int timerId) {
    app.displayCurrentTime();
}
void readPhotoResistor(int timerId) {
#ifdef ENABLE_PHOTORESISTOR
    app.readPhotoresitor();
#endif
}
void neoPixelRefresh(int timerId) {
    app.neoPixelRefresh();
}
void neoPixelNextEffect(int timerId) {
    app.neoPixelNextEffect();
}

void setup() {
    Serial.begin(9600);
#ifdef TEENSYDUINO
    setSyncProvider(getTeensy3Time);
#endif
    Serial.println("Clock-A-Roma v1.0");

    Serial.println("Moving on to setup...");

    app.setup();

    Serial.println("Establishing callbacks...");

    app.rotary->button.attachClick(rotaryButtonClick);
    app.rotary->button.attachLongPressStart(rotaryButtonLongPress);
    app.rotary->button.attachDoubleClick(rotaryButtonDoubleClick);

    timer.setInterval( 1000, displayTimeNow);
    timer.setInterval( 2000, readPhotoResistor);
    timer.setInterval( 5000, neoPixelNextEffect);
    timer.setInterval(    5, neoPixelRefresh);

    Serial.print("Checking time chip and current time... ");

#ifdef ENABLE_SET_TIME
#ifdef TEENSYDUINO
    if (year() < 2014) {
        Serial.println("[BLOOPERS!]");
        Serial.prhint("Year returned is < 2014, resetting time to compile time, 12:00. Year: "); Serial.println(year());
        app.helper.setDateToCompileTime();
    } else {
        Serial.println("[OK]");
    }
#else
    tmElements_t tm;
    bool rtcResult = RTC.read(tm);
    if (RTC.chipPresent()) {
        if (!rtcResult) {
            Serial.println("RTC.read() returned false, resetting to compile time.");
            app.helper.setDateToCompileTime();
        } else {
            sprintf(buffer, "RTC.read() successful, booting at %d/%d/%d %d:%d", tm.Month, tm.Day, tm.Year, tm.Hour, tm.Second);
            Serial.println(buffer);
        }
    } else {
        Serial.println("RTC chip was not detected.");
    }
#endif /* TEENSYDUINO */
#endif /* ENABLE_SET_TIME */

#ifdef ENABLE_NEOPIXELS
    NeoPixelEffects *n = app.neoPixelManager->effects();
    n->reset();
    for (int i = 0; i < 3 * 255; i++) {
        n->fadeCycle();
        delay(2);
    }
    n->fadeOut(2000);
    app.neoPixelManager->shutoff();
    Serial.println("Setup complete, entering loop...");
#endif
}

void loop() {
    timer.run();
    app.run();
    delay(10);
}
