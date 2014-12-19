/*
 * SetTimeMenu.cpp
 *
 *  Created on: Dec 19, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#include "SetTimeMenu.h"

SetTimeMenu::SetTimeMenu(WallClockApp *application) {
    app = application;
    h = m = 0;
}

void SetTimeMenu::nextMode() {
    if (app->mode == SetTime::Last)
        app->mode = SetTime::Default;
    else
        app->mode = (SetTime::TimeMode) ((int) app->mode << 1);
}
void SetTimeMenu::instructions() {
    app->debugLCD(2, "       Click to Save", false);
    app->debugLCD(3, "     DblClck to Exit", false);
}

char *what;

void SetTimeMenu::configureTime() {
    app->blinkLED();
    nextMode();
    tmElements_t tm;
    if (RTC.read(tm)) {
        h = tm.Hour % 12;
        if (h == 0) { h = 12; }
        m = tm.Minute;
    } else {
        app->debugLCD(0, "Can't read current time", true);
        delay(5000);
        return;
    }
    switch (app->mode) {
    case SetTime::Hour:
        app->debugLCD(0, "====== Setup =======", true);
        instructions();
        what = (char *)"Hours";
        selectNumber(&h, 1, 12);
        if (app->mode == SetTime::Default)
            break;
        /* no break */
    case SetTime::Minute:
        app->blinkLED();
        what = (char *)"Minutes";
        selectNumber(&m, 0, 59);
        if (app->mode == SetTime::Default)
            break;
        /* no break */
    case SetTime::Save:
        app->blinkLED();
        tm.Hour = h;
        tm.Minute = m;
        sprintf(app->buf, "New Time: %2d:%02d", h, m);
        Serial.print("Setting Time to: ");
        Serial.println(app->buf);
        app->debugLCD(0, "Saving New Time...", true);
        app->debugLCD(1, app->buf, false);
        if (RTC.write(tm)) {
            app->debugLCD(3, "Success! :)", false);
        } else {
            app->debugLCD(3, "Error :(", false);
        }
        app->helper->setTimeTo(tm);
        nextMode();
        delay(2000);
        /* no break */
    case SetTime::Last:
        app->mode = SetTime::Default;
        /* no break */
    case SetTime::Default:
        digitalWrite(app->config.pinLED, LOW);
        return;
        break;
    };
}

void SetTimeMenu::selectNumber(signed short *current, int min, int max) {
    SetTime::TimeMode startMode = app->mode;
    while (app->mode == startMode) {
        app->rotaryButton->tick();
        signed int delta = app->rotary->rotaryDelta();
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
            sprintf(app->buf, "%-7s: %2d:%02d", what, h, m);
            app->debugLCD(1, app->buf, false);
            delay(30);
        }
    }
}

