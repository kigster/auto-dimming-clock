/*
 * SetTimeMenu.h
 *
 *  Created on: Dec 19, 2014
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#include "../Wallock.h"
#ifdef ENABLE_MENU

#ifndef SETTIMEMENU_H_
#define SETTIMEMENU_H_
namespace Wallock {
    class App;

    class SetTimeMenu {
    public:
        SetTimeMenu();
        SetTimeMenu(Wallock::App *app);
        void setApp(Wallock::App *app);
        void configureTime() ;
        void nextMode();
    private:

        signed short h, m;
        App *app;
        char *what;
        void instructions();
        void updateTimeCallback();
        void selectNumber(signed short *current, int min, int max);

    };
}
#endif /* SETTIMEMENU_H_ */
#endif /* ENABLE_MENU */
