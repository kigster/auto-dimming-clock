#include "WallClock.h"
#include <Arduino.h>

void displayTimeNow(int timerId) {
    tmElements_t tm;
    if (RTC.read(tm)) {
        h = tm.Hour % 12;
        if (h == 0) { h = 12; }
        m = tm.Minute;
        displayTime(h, m);
        sprintf(buf, "%2d:%02d:%02d %d/%02d/%d", h, m, tm.Second, tm.Month, tm.Day, 1970 + tm.Year);
        Serial.println(buf);
        lcd.setCursor(0,3);
        lcd.print(buf);
    } else {
        matrix.print(3333, 10);
    }
}

/**
 * We receive negative hours or minutes when the other
 * element is being setup / modified.
 */
void displayTime(signed short h, signed short m) {
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
    if (m > 0) {
        matrix.writeDigitNum(3, m / 10, false);
        matrix.writeDigitNum(4, m % 10, false);
    }
    matrix.writeDisplay();
    digitalWrite(pinLED, colonOn ? HIGH : LOW);
}


void puts(int row, const char *message, bool clear) {
    if (clear)
        lcd.clear();
    row = row % 4;
    lcd.setCursor(0, row);
    lcd.print(message);
}

void saveNewTime(tmElements_t tm) {
    time_t compileTime = makeTime(tm);
    setTime(compileTime);
    if (RTC.write(tm)) {
        puts(2, "Success!", false);
    } else {
        puts(2, "Failure!", false);
    }
}

void nextMode() {
    if (mode == SetTime::Last)
        mode = SetTime::Default;
    else
        mode = (SetTime::TimeMode) ((int) mode << 1);
}

void instructions() {
    puts(2, "       Click to Save", false);
    puts(3, "     DblClck to Exit", false);
}

char *what;

void configureTime() {
    nextMode();
    tmElements_t tm;
    if (RTC.read(tm)) {
        h = tm.Hour % 12;
        if (h == 0) { h = 12; }
        m = tm.Minute;
    } else {
        puts(0, "Can't read current time", true);
        delay(5000);
        return;
    }
    switch (mode) {
    case SetTime::Hour:
        puts(0, "====== Setup =======", true);
        instructions();
        what = (char *)"Hours";
        updateTimeCallback();
        selectNumber(&h, 1, 12, updateTimeCallback);
        if (mode == SetTime::Default)
            break;
        /* no break */
    case SetTime::Minute:
        what = (char *)"Minutes";
        updateTimeCallback();
        selectNumber(&m, 0, 59, updateTimeCallback);
        if (mode == SetTime::Default)
            break;
        /* no break */
    case SetTime::Save:
        tm.Hour = h;
        tm.Minute = m;
        sprintf(buf, "New Time: %2d:%02d", h, m);
        Serial.print("Setting Time to: ");
        Serial.println(buf);
        puts(0, "Saving New Time...", true);
        puts(1, buf, false);
        if (RTC.write(tm)) {
            puts(3, "Success! :)", false);
        } else {
            puts(3, "Error :(", false);
        }
        saveNewTime(tm);
        nextMode();
        delay(2000);
        /* no break */
    case SetTime::Last:
        mode = SetTime::Default;
        /* no break */
    case SetTime::Default:
        return;
        break;
    };
}

void updateTimeCallback() {
    displayTime(
            (mode == SetTime::Hour ? h : -1),
            (mode == SetTime::Minute ? m : -1)
            );
    sprintf(buf, "%-7s: %2d:%02d", what, h, m);
    puts(1, buf, false);
}

void selectNumber(signed short *current, int min, int max,
        displayUpdateCallback callback) {
    SetTime::TimeMode startMode = mode;
    while (mode == startMode) {
        button.tick();
        signed int delta = rotary.rotaryDelta();
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
            callback();
            delay(30);
        }
    }
}

