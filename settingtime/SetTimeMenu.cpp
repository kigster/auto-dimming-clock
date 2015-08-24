/*
 * SetTimeMenu.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */
#include "../Wallock.h"
#if ENABLE_MENU
#include "SetTimeMenu.h"
#include "../app/App.h"

namespace Wallock {
    SetTimeMenu::SetTimeMenu(Wallock::App *application) {
        app = application;
        h = m = 0;
        what = (char *) "";
        timeSinceLastModeChange = 0;
    }

    SetTimeMenu::SetTimeMenu() {
        app = NULL;
        h = m = 0;
        what = (char *) "";
        timeSinceLastModeChange = 0;
    }

    void SetTimeMenu::setApp(Wallock::App *application) {
        app = application;
    }

    void SetTimeMenu::nextMode() {
//        long now = millis();
//        app->getButton()->tick();
//        if (now - timeSinceLastModeChange < 1000)
//            return;
//        timeSinceLastModeChange = now;

        if  (app->mode == SetTime::Last) app->mode = SetTime::Default;
        else app->mode = (SetTime::TimeMode) ((int) app->mode << 1);
        printf("Mode Changed, new mode: %d", (int) app->mode);
    }
    void SetTimeMenu::instructions() {
        app->debug(2, "       Click to Save", false);
        app->debug(3, "     DblClck to Exit", false);
    }

    void SetTimeMenu::configureTime() {
        Serial.println(F("Entering configureTime()..."));
        nextMode();
        tmElements_t tm;
        #if TEENSYDUINO
            breakTime(now(), tm);
        #else
            if (!RTC.read(tm)) {
                app->debug(0, "Can't ¨read current time", true);
                delay(3000);
                return;
            }
        #endif
        switch (app->mode) {
            case SetTime::Hour:
                app->debug(0, "====== Setup =======", true);
                instructions();
                what = (char *) "Hours";
                h = tm.Hour;
                if (h == 0) { h = 24; }
                selectNumber(&h, 1, 24);
                if (app->mode == SetTime::Default) break;
                /* no break */

            case SetTime::Minute:
                what = (char *) "Minutes";
                m = tm.Minute;
                selectNumber(&m, 0, 59);
                if (app->mode == SetTime::Default) break;
                /* no break */

            case SetTime::Save:
                if (h == 24) { h = 0; }
                tm.Hour = h;
                tm.Minute = m;
                tm.Second = 0;
                sprintf(buffer, "New Time: %2d:%02d", h, m);
                Serial.print("Setting Time to: ");
                Serial.println(buffer);
                #if DEBUG
                    app->debug(0, "Saving New Time...", true);
                    app->debug(1, buffer, false);
                    app->debug(3, "Success! :)", false);
                #endif
                app->helper.setTimeTo(tm);
                nextMode();
                /* no break */

            case SetTime::Last:
                app->mode = SetTime::Default;
                /* no break */

            case SetTime::Default:
                return;
        };
    }

    void SetTimeMenu::selectNumber(signed short *current, int min, int max) {
        SetTime::TimeMode startMode = app->mode;
        Serial.print(F("Entering: selectNumber()"));
        Serial.println((int) startMode);
        app->displayTime((app->mode == SetTime::Hour ? h : -1), (app->mode == SetTime::Minute ? m : -1));
        while (app->mode == startMode) {
            app->getRotary()->tick();
            int delta = app->getRotary()->delta();
            if (abs(delta) < 2) {
                delay(20);
                continue;
            }

            delta = (delta > 0) ? 1 : -1;

            int prev = *current;
            *current = *current + delta;
            if (*current > max) *current = *current % max;
            if (*current < min) *current = max;
            if (prev != *current) {
                app->displayTime((app->mode == SetTime::Hour ? h : -1),
                                 (app->mode == SetTime::Minute ? m : -1));
                sprintf(buffer, "%-7s: %2d:%02d", what, h, m);
                app->debug(1, buffer, false);
                delay(5);
            }
        }
        Serial.print(F("Exiting: selectNumber() "));
        Serial.println((int) startMode);
    }
}
#endif /* ENABLE_MENU */
