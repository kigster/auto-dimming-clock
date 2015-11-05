/*
 * color.h
 *
 *  Created on: Sep 6, 2015
 *      Author: Konstantin Gredeskoul
 *        Code: https://github.com/kigster
 *
 *  (c) 2014 All rights reserved, MIT License.
 */

#ifndef APP_COLORMANAGER_H_
#define APP_COLORMANAGER_H_

#if ENABLE_ENCODER_RGB
namespace Wallock {

class ColorManager {
    uint8_t colors[3];

public:
    ColorManager(uint8_t _r, uint8_t _b, uint8_t _g) {
        colors[0] = _r;
        colors[1] = _g;
        colors[2] = _b;
    };

    void init() {
        for (int i = 0; i < 3; i++) {
            digitalWrite(colors[i], LOW);
            fadeColor(i, 0, 0xFF >> 1, 1);
            fadeColor(i, 0xFF >> 1, 0, 1);
        }
    }

    void fadeColor(uint8_t pin_id, uint8_t from, uint8_t to, uint8_t delay_ms) {
        for (uint8_t j = from; (to > from) ? j < to : j > to; (to > from) ? j++ : j--) {
            analogWrite(colors[pin_id], j);
            if (delay_ms > 0) delay(delay_ms);
        }
    }

    void setColor(long _color) {
        setColor((_color & 0xFF0000) >> 020,
                 (_color & 0x00FF00) >> 010,
                  _color & 0x0000FF);
    }

    void setColor(uint8_t red, uint8_t green, uint8_t blue) {
        analogWrite(colors[0], red);
        analogWrite(colors[1], green);
        analogWrite(colors[2], blue);
    }

    void off() {
        setColor(0,0,0);
    }

};

namespace Colors {
    const long steelblue = 0x4682B4;
    const long royalblue = 0x041690;
    const long cornflowerblue = 0x6495ED;
    const long lightsteelblue = 0xB0C4DE;
    const long mediumslateblue = 0x7B68EE;
    const long slateblue = 0x6A5ACD;
    const long darkslateblue = 0x483D8B;
    const long midnightblue = 0x191970;
    const long navy = 0x000080;
    const long darkblue = 0x00008B;
    const long mediumblue = 0x0000CD;
    const long blue = 0x0000FF;
    const long dodgerblue = 0x1E90FF;
    const long deepskyblue = 0x00BFFF;
    const long lightskyblue = 0x87CEFA;
    const long skyblue = 0x87CEEB;
    const long lightblue = 0xADD8E6;
    const long powderblue = 0xB0E0E6;
    const long azure = 0xF0FFFF;
    const long lightcyan = 0xE0FFFF;
    const long paleturquoise = 0xAFEEEE;
    const long mediumturquoise = 0x48D1CC;
    const long lightseagreen = 0x20B2AA;
    const long darkcyan = 0x008B8B;
    const long teal = 0x008080;
    const long cadetblue = 0x5F9EA0;
    const long darkturquoise = 0x00CED1;
    const long aqua = 0x00FFFF;
    const long cyan = 0x00FFFF;
    const long turquoise = 0x40E0D0;
    const long aquamarine = 0x7FFFD4;
    const long mediumaquamarine = 0x66CDAA;
    const long darkseagreen = 0x8FBC8F;
    const long mediumseagreen = 0x3CB371;
    const long seagreen = 0x2E8B57;
    const long darkgreen = 0x006400;
    const long green = 0x008000;
    const long forestgreen = 0x228B22;
    const long limegreen = 0x32CD32;
    const long lime = 0x00FF00;
    const long chartreuse = 0x7FFF00;
    const long lawngreen = 0x7CFC00;
    const long greenyellow = 0xADFF2F;
    const long yellowgreen = 0x9ACD32;
    const long palegreen = 0x98FB98;
    const long lightgreen = 0x90EE90;
    const long springgreen = 0x00FF7F;
    const long mediumspringgreen = 0x00FA9A;
    const long darkolivegreen = 0x556B2F;
    const long olivedrab = 0x6B8E23;
    const long olive = 0x808000;
    const long darkkhaki = 0xBDB76B;
    const long darkgoldenrod = 0xB8860B;
    const long goldenrod = 0xDAA520;
    const long gold = 0xFFD700;
    const long yellow = 0xFFFF00;
    const long khaki = 0xF0E68C;
    const long palegoldenrod = 0xEEE8AA;
    const long blanchedalmond = 0xFFEBCD;
    const long moccasin = 0xFFE4B5;
    const long wheat = 0xF5DEB3;
    const long navajowhite = 0xFFDEAD;
    const long burlywood = 0xDEB887;
    const long tan = 0xD2B48C;
    const long rosybrown = 0xBC8F8F;
    const long sienna = 0xA0522D;
    const long saddlebrown = 0x8B4513;
    const long chocolate = 0xD2691E;
    const long peru = 0xCD853F;
    const long sandybrown = 0xF4A460;
    const long darkred = 0x8B0000;
    const long maroon = 0x800000;
    const long brown = 0xA52A2A;
    const long firebrick = 0xB22222;
    const long indianred = 0xCD5C5C;
    const long lightcoral = 0xF08080;
    const long salmon = 0xFA8072;
    const long darksalmon = 0xE9967A;
    const long lightsalmon = 0xFFA07A;
    const long coral = 0xFF7F50;
    const long tomato = 0xFF6347;
    const long darkorange = 0xFF8C00;
    const long orange = 0xFFA500;
    const long orangered = 0xFF4500;
    const long crimson = 0xDC143C;
    const long red = 0xFF0000;
    const long deeppink = 0xFF1493;
    const long fuchsia = 0xFF00FF;
    const long magenta = 0xFF00FF;
    const long hotpink = 0xFF69B4;
    const long lightpink = 0xFFB6C1;
    const long pink = 0xFFC0CB;
    const long palevioletred = 0xDB7093;
    const long mediumvioletred = 0xC71585;
    const long purple = 0x800080;
    const long darkmagenta = 0x8B008B;
    const long mediumpurple = 0x9370DB;
    const long blueviolet = 0x8A2BE2;
    const long indigo = 0x4B0082;
    const long darkviolet = 0x9400D3;
    const long darkorchid = 0x9932CC;
    const long mediumorchid = 0xBA55D3;
    const long orchid = 0xDA70D6;
    const long violet = 0xEE82EE;
    const long plum = 0xDDA0DD;
    const long thistle = 0xD8BFD8;
    const long lavender = 0xE6E6FA;
    const long ghostwhite = 0xF8F8FF;
    const long aliceblue = 0xF0F8FF;
    const long mintcream = 0xF5FFFA;
    const long honeydew = 0xF0FFF0;
    const long lightgoldenrodyellow = 0xFAFAD2;
    const long lemonchiffon = 0xFFFACD;
    const long cornsilk = 0xFFF8DC;
    const long lightyellow = 0xFFFFE0;
    const long ivory = 0xFFFFF0;
    const long floralwhite = 0xFFFAF0;
    const long linen = 0xFAF0E6;
    const long oldlace = 0xFDF5E6;
    const long antiquewhite = 0xFAEBD7;
    const long bisque = 0xFFE4C4;
    const long peachpuff = 0xFFDAB9;
    const long papayawhip = 0xFFEFD5;
    const long beige = 0xF5F5DC;
    const long seashell = 0xFFF5EE;
    const long lavenderblush = 0xFFF0F5;
    const long mistyrose = 0xFFE4E1;
    const long snow = 0xFFFAFA;
    const long white = 0xFFFFFF;
    const long whitesmoke = 0xF5F5F5;
    const long gainsboro = 0xDCDCDC;
    const long lightgrey = 0xD3D3D3;
    const long silver = 0xC0C0C0;
    const long darkgray = 0xA9A9A9;
    const long gray = 0x808080;
    const long lightslategray = 0x778899;
    const long slategray = 0x708090;
    const long dimgray = 0x696969;
    } /* namespace Color */
} /* namespace Wallock */

#endif /* ENABLE_ENCODER_RGB */
#endif /* APP_COLORMANAGER_H_ */
