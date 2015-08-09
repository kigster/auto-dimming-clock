/*
 * NeoPixelEffects.cpp
 *
 *  Created on: Nov 28, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */
#include "../Wallock.h"
#ifdef ENABLE_NEOPIXELS

#include "NeoPixelEffects.h"


NeoPixelEffects::NeoPixelEffects() {
    _numEffectsEnabled = 0;
    _currentEffectIndex = 0;
    _currentEffect = NULL;
    reset();

    void (NeoPixelEffects::*allEffects[])() = {
//        &NeoPixelEffects::theaterChaseRainbow,
//        &NeoPixelEffects::randomColor,
        &NeoPixelEffects::rainbow,
        &NeoPixelEffects::rainbowCycle,
        NULL
    };

    for (int q = 0; allEffects[q] != NULL; q++) {
        _enabledEffects[q] = allEffects[q];
        _numEffectsEnabled++;
    }
    _numEffectsEnabled %= MAX_EFFECTS;
    chooseNewEffect();
}
void NeoPixelEffects::setStrip(Adafruit_NeoPixel *strip) {
    _strip = strip;

}
void NeoPixelEffects::reset() {
    i = 0;
    j = 0;
    flag = false;
}

void NeoPixelEffects::chooseNewEffect() {
    if (effectsEnabled() > 0) {
        _currentEffectIndex = random(effectsEnabled());
        _currentEffect = _enabledEffects[_currentEffectIndex];
        i = 0;
        j = 0;
        //printv("Next effect #", _currentEffectIndex);
    }
}

int NeoPixelEffects::effectsEnabled() {
    return _numEffectsEnabled;
}
int NeoPixelEffects::currentEffectIndex() {
    return _currentEffectIndex;
}

void NeoPixelEffects::refreshCurrentEffect() {
    if (effectsEnabled() > 0 && _currentEffect != NULL) {
        (this->*((NeoPixelEffects*) this)->NeoPixelEffects::_currentEffect)();
    }
}
void NeoPixelEffects::fadeCycle() {
    const static int max = 127;
    signed int r, g, b;
    if (i > max) {
        flag = true;
        i = 0;
    }
    r = (flag) ? i : -i;
    g = (flag) ? i - max / 2 : max / 2 - i;
    b = (flag) ? i - max : max - i;
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    _strip->setPixelColor(j, _strip->Color(r,g,b));
    _strip->show();
    i++;
    if (i > max && flag) {
        i = 0;
        flag = false;
        j++;
        j = j % 3;
    }
}

void NeoPixelEffects::fadeOut(long millis) {
    int delayTime = millis / 256;
    uint8_t r, g, b;
    for (i = 255; i >= 0; i--) {
        for (int p = 0; p < _strip->numPixels(); p++) {
            uint32_t c = _strip->getPixelColor(p);
            r = (uint8_t)(c >> 16);
            g = (uint8_t)(c >>  8);
            b = (uint8_t) c;
            if (r > 0) r--;
            if (g > 0) g--;
            if (b > 0) b--;
            _strip->setPixelColor(p, _strip->Color(r,g,b));
        }
        _strip->show();
        if ((r + g + b) == 0) break;
        delay(delayTime);
    }
}
void NeoPixelEffects::randomColor() {
    i++;
    switch ((i / 20) % 4) {
    case 0:
        colorBlue();
        break;
    case 1:
        colorRed();
        break;
    case 2:
        colorGreen();
        break;
    case 3:
        colorYellow();
        break;
    }
}
void NeoPixelEffects::colorRed() {
    colorWipe(_strip->Color(127, 0, 0)); // Red
}
void NeoPixelEffects::colorBlue() {
    colorWipe(_strip->Color(0, 0, 127)); // Blue
}
void NeoPixelEffects::colorGreen() {
    colorWipe(_strip->Color(0, 127, 0)); // Blue
}
void NeoPixelEffects::colorYellow() {
    colorWipe(_strip->Color(127, 127, 0)); // Yellow
}

// Fill the dots one after the other with a color
void NeoPixelEffects::colorWipe(uint32_t c) {
    for (int p = 0; p < _strip->numPixels(); p++) {
        _strip->setPixelColor(p, c);
    }
    _strip->show();
}

void NeoPixelEffects::rainbow() {
    for (i = 0; i < _strip->numPixels(); i++) {
        _strip->setPixelColor(i, Wheel((i + j) & 255));
    }
    _strip->show();
    j++;
    j %= 256;
}

// Slightly different, this makes the rainbow equally distributed throughout
void NeoPixelEffects::rainbowCycle() {
    for (i = 0; i < _strip->numPixels(); i++) {
        _strip->setPixelColor(i,
                Wheel(((i * 256 / _strip->numPixels()) + j) & 255));
    }
    _strip->show();
    j++;
    j %= 256 * 5;
}

void NeoPixelEffects::theaterChase(uint32_t c) {
    for (int p = 0; p < _strip->numPixels()*2; p = p + 2) {
        _strip->setPixelColor((p) % _strip->numPixels(), c);    //turn every third pixel on
    }
    for (int p = 1; p < _strip->numPixels()*2; p = p + 2) {
        _strip->setPixelColor((p) % _strip->numPixels(), 0);    //turn every third pixel off
    }
    _strip->show();
    j++;
    j %= 10;
    i++;
    i %= 10;
}

//Theatre-style crawling lights with rainbow effect
void NeoPixelEffects::theaterChaseRainbow() {
    uint32_t c = Wheel(j % 255);
    for (int p = 0; p < _strip->numPixels()*2; p = p + 2) {
        _strip->setPixelColor((p) % _strip->numPixels(), c);    //turn every third pixel on
    }
    for (int p = 1; p < _strip->numPixels()*2; p = p + 2) {
        _strip->setPixelColor((p) % _strip->numPixels(), 0);    //turn every third pixel off
    }
    _strip->show();
    j++;
    j %= 10;
    i++;
    i %= 10;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t NeoPixelEffects::Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85) {
        return _strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else if (WheelPos < 170) {
        WheelPos -= 85;
        return _strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
    } else {
        WheelPos -= 170;
        return _strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
}

#endif
