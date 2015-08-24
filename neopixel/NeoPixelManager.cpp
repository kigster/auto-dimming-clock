/*
 * NeoPixelManager.cpp
 *
 *  Created on: Nov 28, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#include "../Wallock.h"
#if ENABLE_NEOPIXELS

#include "NeoPixelManager.h"
#include "NeoPixelEffects.h"
#include <Adafruit_NeoPixel.h>

NeoPixelManager::NeoPixelManager(uint8_t pixels, uint8_t pin) {
    _strip = new Adafruit_NeoPixel(pixels, pin, NEO_GRB + NEO_KHZ800);
    _effects.setStrip(_strip);
    _playing = false;
}

uint32_t NeoPixelManager::color(uint8_t r, uint8_t g, uint8_t b) {
    return _strip->Color(r,g,b);
}

void NeoPixelManager::shutoff() {
    _playing = false;
    _strip->clear();
    _strip->show();
}

void NeoPixelManager::begin() {
    _strip->begin();
    _strip->clear();
    _strip->show(); // Initialize all pixels to 'off'
}

void NeoPixelManager::nextEffect() {
    _playing = true;
    _effects.chooseNewEffect();
}

void NeoPixelManager::refreshEffect() {
    _playing = true;
    _effects.refreshCurrentEffect();
}

NeoPixelEffects *NeoPixelManager::effects() {
    return &_effects;
}

bool NeoPixelManager::playing() {
    return _playing;
}

#endif
