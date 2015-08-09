/*
 * SetTimeMenu.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */
#include "../BedTime.h"
#ifdef ENABLE_MENU
#include "SetTimeMenu.h"
#include "../app/BedTimeApp.h"

SetTimeMenu::SetTimeMenu(BedTimeApp *application) {
    app = application;
    h = m = 0;
    what = "";
}

SetTimeMenu::SetTimeMenu() {
    app = NULL;
    h = m = 0;
    what = "";
}

void SetTimeMenu::setApp(BedTimeApp *application) {
    app = application;
}

void SetTimeMenu::nextMode() {
    if (app->mode == SetTime::Last)
        app->mode = SetTime::Default;
    else
        app->mode = (SetTime::TimeMode) ((int) app->mode << 1);
    printf("Mode Changed, new mode: %d", (int) app->mode);
}
void SetTimeMenu::instructions() {
    app->debug(2, "       Click to Save", false);
    app->debug(3, "     DblClck to Exit", false);
}

void SetTimeMenu::configureTime() {
    Serial.println("Entering setup...");
    nextMode();
    tmElements_t tm;
#ifdef TEENSYDUINO
    breakTime(now(), tm);
#else
    if (RTC.read(tm)) {
        h = tm.Hour % 12;
        if (h == 0) { h = 12; }
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
        what = (char *)"Hours";
        h = tm.Hour;
        selectNumber(&h, 1, 12);
        if (app->mode == SetTime::Default)
            break;
        /* no break */
    case SetTime::Minute:
        what = (char *)"Minutes";
        m = tm.Minute;
        selectNumber(&m, 0, 59);
        if (app->mode == SetTime::Default)
            break;
        /* no break */
    case SetTime::Save:
        tm.Hour = h;
        tm.Minute = m;
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
    Serial.print("enter: startMode = "); Serial.println((int)startMode);
    app->displayTime(
                      (app->mode == SetTime::Hour ? h : -1),
                      (app->mode == SetTime::Minute ? m : -1)
                      );
    while (app->mode == startMode) {
        app->rotary->button.tick();
        signed short delta = app->rotary->delta();
        if (abs(delta) < 2) {
            delay(20);
            continue;
        }
        delta = (delta > 0) ? 1 : -1;
        int prev = *current;
        *current = *current + delta;
        if (*current > max)
            *current = max;
        if (*current < min)
            *current = min;
        if (prev != *current) {
            app->displayTime(
                    (app->mode == SetTime::Hour ? h : -1),
                    (app->mode == SetTime::Minute ? m : -1)
                    );
            sprintf(buffer, "%-7s: %2d:%02d", what, h, m);
            app->debug(1, buffer, false);
            delay(30);
        }
    }
    Serial.print("exit: startMode = "); Serial.println((int)startMode);
}

#endif /* ENABLE_MENU */
