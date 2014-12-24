/*
 * NeoPixelEffects.h
 *
 *  Created on: Nov 28, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#ifndef NEOPIXELEFFECTS_H_
#define NEOPIXELEFFECTS_H_
#include <Adafruit_NeoPixel.h>

#define MAX_EFFECTS 20



class NeoPixelEffects {
public:
    NeoPixelEffects(Adafruit_NeoPixel *strip);
    ~NeoPixelEffects();
    int effectsEnabled();
    int currentEffectIndex();

    // effects
    void colorRed(),
        colorBlue(),
        colorGreen(),
        colorYellow(),
        randomColor(),
        rainbow(),
        rainbowCycle(),
        theaterChaseRainbow();

    // helpers
    void colorWipe(uint32_t c),
        theaterChase(uint32_t c),
        chooseNewEffect(),
        refreshCurrentEffect();


    uint32_t Wheel(byte WheelPos);

private:
    Adafruit_NeoPixel *_strip;
    void (NeoPixelEffects::*_enabledEffects[MAX_EFFECTS])();
    void (NeoPixelEffects::*_currentEffect)();
    int _numEffectsEnabled;
    int _currentEffectIndex;
    uint16_t i, j;

};

inline NeoPixelEffects::~NeoPixelEffects() {};


#endif /* NEOPIXELEFFECTS_H_ */
