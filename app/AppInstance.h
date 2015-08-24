/*
 * AppInstance.h
 *
 *  Created on: Aug 23, 2015
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#ifndef APP_APPINSTANCE_H_
#define APP_APPINSTANCE_H_
#include <Arduino.h>

#include "App.h"

namespace Wallock {

    static App *appInstance;

    static void callBack_buttonDoubleClick() {
        if (appInstance != NULL) appInstance->eventDblClick();
    }
    static void callBack_buttonClick() {
        if (appInstance != NULL) appInstance->eventClick();
    }
    static void callBack_buttonHold() {
        if (appInstance != NULL) appInstance->eventHold();
    }
    static void callBack_displayCurrentTime() {
        if (Wallock::appInstance != NULL) appInstance->displayCurrentTime();
    }
    static void callBack_neoPixelRefresh() {
        if (appInstance != NULL) appInstance->neoPixelRefresh();
    }
    static void callBack_neoPixelNextEffect() {
        if (appInstance != NULL) appInstance->neoPixelNextEffect();
    }
}

#endif /* APP_APPINSTANCE_H_ */
