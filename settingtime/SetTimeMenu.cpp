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
#ifdef ENABLE_MENU
#include "SetTimeMenu.h"
#include "../app/App.h"

namespace Wallock {
    SetTimeMenu::SetTimeMenu(Wallock::App *application) {
        app = application;
        h = m = 0;
        what = (char *) "";
    }

    SetTimeMenu::SetTimeMenu() {
        app = NULL;
        h = m = 0;
        what = (char *) "";
    }

    void SetTimeMenu::setApp(Wallock::App *application) {
        app = application;
    }

    void SetTimeMenu::nextMode() {
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
    #ifdef TEENSYDUINO
        breakTime(now(), tm);
    #else
        if (RTC.read(tm)) {
            h = tm.Hour % app->maxHour();
            if (h == 0) {
                h = app->maxHour();
            }
            m = tm.Minute;
        } else {
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
            selectNumber(&h, 1, app->maxHour());
            if (app->mode == SetTime::Default) break;
            /* no break */
        case SetTime::Minute:
            what = (char *) "Minutes";
            m = tm.Minute;
            selectNumber(&m, 0, 59);
            if (app->mode == SetTime::Default) break;
            /* no break */
        case SetTime::Save:
            tm.Hour = h;
            tm.Minute = m;
            tm.Second = 0;
            sprintf(buffer, "New Time: %2d:%02d", h, m);
            Serial.print("Setting Time to: ");
            Serial.println(buffer);
            app->debug(0, "Saving New Time...", true);
            app->debug(1, buffer, false);
            app->debug(3, "Success! :)", false);
            app->helper.setTimeTo(tm);
            nextMode();
            delay(1000);
            /* no break */
        case SetTime::Last:
            app->mode = SetTime::Default;
            /* no break */
        case SetTime::Default:
            return;
            break;
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
                app->displayTime((app->mode == SetTime::Hour ? h : -1), (app->mode == SetTime::Minute ? m : -1));
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
