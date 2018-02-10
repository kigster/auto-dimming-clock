#ifndef COLOR_H_
#define COLOR_H_

#include <stdint.h>
#include <Arduino.h>

typedef uint8_t color_t;
typedef uint32_t rgb_color_t;

#define converge(X,Y,f) ((int)X + (int)(f * (max(X,Y) - min(X,Y)) * (Y > X ? 1.0 : -1.0)))

class RGBColor {
public:

    color_t r;
    color_t g;
    color_t b;

    RGBColor() :
            r(0), g(0), b(0) {
    }
    RGBColor(color_t r, color_t g, color_t b) :
            r(r), g(g), b(b) {
    }
    RGBColor(rgb_color_t p) {
      r = (p >> 16) & 0xff;
      g = (p >> 8) & 0xff;
      b = p & 0xff;
    }

    RGBColor operator-(const RGBColor c) {
      return RGBColor(r - c.r, g - c.g, b - c.b);
    }

    RGBColor operator+(const RGBColor c) {
      return RGBColor(r + c.r, g + c.g, b + c.b);
    }

    RGBColor operator+(const uint32_t c) {
      color_t cr = (c >> 16) & 0xff;
      color_t cg = (c >> 8) & 0xff;
      color_t cb = c & 0xff;
      return RGBColor(r + cr, g + cg, b + cb);
    }

    void operator-=(const RGBColor c) {
      r -= c.r;
      g -= c.g;
      b -= c.b;
    }

    void operator+=(const RGBColor c) {
      r += c.r;
      g += c.g;
      b += c.b;
    }

    RGBColor operator*(const float f) {
      color_t tR = r * f;
      color_t tG = g * f;
      color_t tB = b * f;
      if (tR == 0 && r != 0) tR = 1;
      if (tG == 0 && g != 0) tG = 1;
      if (tB == 0 && b != 0) tB = 1;

      return RGBColor(tR, tG, tB);
    }

    /* expecting 0 <= f <= 1 */
    RGBColor scaleTo(const RGBColor other, const float ratio) {
      color_t tR = converge(r, other.r, ratio);
      color_t tG = converge(g, other.g, ratio);
      color_t tB = converge(b, other.b, ratio);
      return RGBColor(tR, tG, tB);
    }

    RGBColor operator/(const int f) {
      return RGBColor(r / f, g / f, b / f);
    }
};

#if LOAD_COLOR_NAMES

namespace ColorNames {
  const PROGMEM rgb_color_t aliceblue                  =  0xF0F8FF;
  const PROGMEM rgb_color_t antiquewhite               =  0xFAEBD7;
  const PROGMEM rgb_color_t aqua                       =  0x00FFFF;
  const PROGMEM rgb_color_t aquamarine                 =  0x7FFFD4;
  const PROGMEM rgb_color_t azure                      =  0xF0FFFF;
  const PROGMEM rgb_color_t beige                      =  0xF5F5DC;
  const PROGMEM rgb_color_t bisque                     =  0xFFE4C4;
  const PROGMEM rgb_color_t blanchedalmond             =  0xFFEBCD;
  const PROGMEM rgb_color_t black                      =  0x000000;
  const PROGMEM rgb_color_t blue                       =  0x0000FF;
  const PROGMEM rgb_color_t blueviolet                 =  0x8A2BE2;
  const PROGMEM rgb_color_t brown                      =  0xA52A2A;
  const PROGMEM rgb_color_t burlywood                  =  0xDEB887;
  const PROGMEM rgb_color_t cadetblue                  =  0x5F9EA0;
  const PROGMEM rgb_color_t chartreuse                 =  0x7FFF00;
  const PROGMEM rgb_color_t chocolate                  =  0xD2691E;
  const PROGMEM rgb_color_t coral                      =  0xFF7F50;
  const PROGMEM rgb_color_t cornflowerblue             =  0x6495ED;
  const PROGMEM rgb_color_t cornsilk                   =  0xFFF8DC;
  const PROGMEM rgb_color_t crimson                    =  0xDC143C;
  const PROGMEM rgb_color_t cyan                       =  0x00FFFF;
  const PROGMEM rgb_color_t darkblue                   =  0x00008B;
  const PROGMEM rgb_color_t darkcyan                   =  0x008B8B;
  const PROGMEM rgb_color_t darkgoldenrod              =  0xB8860B;
  const PROGMEM rgb_color_t darkgray                   =  0xA9A9A9;
  const PROGMEM rgb_color_t darkgreen                  =  0x006400;
  const PROGMEM rgb_color_t darkkhaki                  =  0xBDB76B;
  const PROGMEM rgb_color_t darkmagenta                =  0x8B008B;
  const PROGMEM rgb_color_t darkolivegreen             =  0x556B2F;
  const PROGMEM rgb_color_t darkorange                 =  0xFF8C00;
  const PROGMEM rgb_color_t darkorchid                 =  0x9932CC;
  const PROGMEM rgb_color_t darkred                    =  0x8B0000;
  const PROGMEM rgb_color_t darksalmon                 =  0xE9967A;
  const PROGMEM rgb_color_t darkseagreen               =  0x8FBC8F;
  const PROGMEM rgb_color_t darkslateblue              =  0x483D8B;
  const PROGMEM rgb_color_t darkturquoise              =  0x00CED1;
  const PROGMEM rgb_color_t darkviolet                 =  0x9400D3;
  const PROGMEM rgb_color_t deeppink                   =  0xFF1493;
  const PROGMEM rgb_color_t deepskyblue                =  0x00BFFF;
  const PROGMEM rgb_color_t dimgray                    =  0x696969;
  const PROGMEM rgb_color_t dodgerblue                 =  0x1E90FF;
  const PROGMEM rgb_color_t firebrick                  =  0xB22222;
  const PROGMEM rgb_color_t floralwhite                =  0xFFFAF0;
  const PROGMEM rgb_color_t forestgreen                =  0x228B22;
  const PROGMEM rgb_color_t fuchsia                    =  0xFF00FF;
  const PROGMEM rgb_color_t gainsboro                  =  0xDCDCDC;
  const PROGMEM rgb_color_t ghostwhite                 =  0xF8F8FF;
  const PROGMEM rgb_color_t gold                       =  0xFFD700;
  const PROGMEM rgb_color_t goldenrod                  =  0xDAA520;
  const PROGMEM rgb_color_t gray                       =  0x808080;
  const PROGMEM rgb_color_t green                      =  0x008000;
  const PROGMEM rgb_color_t greenyellow                =  0xADFF2F;
  const PROGMEM rgb_color_t honeydew                   =  0xF0FFF0;
  const PROGMEM rgb_color_t hotpink                    =  0xFF69B4;
  const PROGMEM rgb_color_t indianred                  =  0xCD5C5C;
  const PROGMEM rgb_color_t indigo                     =  0x4B0082;
  const PROGMEM rgb_color_t ivory                      =  0xFFFFF0;
  const PROGMEM rgb_color_t khaki                      =  0xF0E68C;
  const PROGMEM rgb_color_t lavender                   =  0xE6E6FA;
  const PROGMEM rgb_color_t lavenderblush              =  0xFFF0F5;
  const PROGMEM rgb_color_t lawngreen                  =  0x7CFC00;
  const PROGMEM rgb_color_t lemonchiffon               =  0xFFFACD;
  const PROGMEM rgb_color_t lightblue                  =  0xADD8E6;
  const PROGMEM rgb_color_t lightcoral                 =  0xF08080;
  const PROGMEM rgb_color_t lightcyan                  =  0xE0FFFF;
  const PROGMEM rgb_color_t lightgoldenrodyellow       =  0xFAFAD2;
  const PROGMEM rgb_color_t lightgreen                 =  0x90EE90;
  const PROGMEM rgb_color_t lightgrey                  =  0xD3D3D3;
  const PROGMEM rgb_color_t lightpink                  =  0xFFB6C1;
  const PROGMEM rgb_color_t lightsalmon                =  0xFFA07A;
  const PROGMEM rgb_color_t lightseagreen              =  0x20B2AA;
  const PROGMEM rgb_color_t lightskyblue               =  0x87CEFA;
  const PROGMEM rgb_color_t lightslategray             =  0x778899;
  const PROGMEM rgb_color_t lightsteelblue             =  0xB0C4DE;
  const PROGMEM rgb_color_t lightyellow                =  0xFFFFE0;
  const PROGMEM rgb_color_t lime                       =  0x00FF00;
  const PROGMEM rgb_color_t limegreen                  =  0x32CD32;
  const PROGMEM rgb_color_t linen                      =  0xFAF0E6;
  const PROGMEM rgb_color_t magenta                    =  0xFF00FF;
  const PROGMEM rgb_color_t maroon                     =  0x800000;
  const PROGMEM rgb_color_t mediumaquamarine           =  0x66CDAA;
  const PROGMEM rgb_color_t mediumblue                 =  0x0000CD;
  const PROGMEM rgb_color_t mediumorchid               =  0xBA55D3;
  const PROGMEM rgb_color_t mediumpurple               =  0x9370DB;
  const PROGMEM rgb_color_t mediumseagreen             =  0x3CB371;
  const PROGMEM rgb_color_t mediumslateblue            =  0x7B68EE;
  const PROGMEM rgb_color_t mediumspringgreen          =  0x00FA9A;
  const PROGMEM rgb_color_t mediumturquoise            =  0x48D1CC;
  const PROGMEM rgb_color_t mediumvioletred            =  0xC71585;
  const PROGMEM rgb_color_t midnightblue               =  0x191970;
  const PROGMEM rgb_color_t mintcream                  =  0xF5FFFA;
  const PROGMEM rgb_color_t mistyrose                  =  0xFFE4E1;
  const PROGMEM rgb_color_t moccasin                   =  0xFFE4B5;
  const PROGMEM rgb_color_t navajowhite                =  0xFFDEAD;
  const PROGMEM rgb_color_t navy                       =  0x000080;
  const PROGMEM rgb_color_t oldlace                    =  0xFDF5E6;
  const PROGMEM rgb_color_t olive                      =  0x808000;
  const PROGMEM rgb_color_t olivedrab                  =  0x6B8E23;
  const PROGMEM rgb_color_t orange                     =  0xFFA500;
  const PROGMEM rgb_color_t orangered                  =  0xFF4500;
  const PROGMEM rgb_color_t orchid                     =  0xDA70D6;
  const PROGMEM rgb_color_t palegoldenrod              =  0xEEE8AA;
  const PROGMEM rgb_color_t palegreen                  =  0x98FB98;
  const PROGMEM rgb_color_t paleturquoise              =  0xAFEEEE;
  const PROGMEM rgb_color_t palevioletred              =  0xDB7093;
  const PROGMEM rgb_color_t papayawhip                 =  0xFFEFD5;
  const PROGMEM rgb_color_t peachpuff                  =  0xFFDAB9;
  const PROGMEM rgb_color_t peru                       =  0xCD853F;
  const PROGMEM rgb_color_t pink                       =  0xFFC0CB;
  const PROGMEM rgb_color_t plum                       =  0xDDA0DD;
  const PROGMEM rgb_color_t powderblue                 =  0xB0E0E6;
  const PROGMEM rgb_color_t purple                     =  0x800080;
  const PROGMEM rgb_color_t red                        =  0xFF0000;
  const PROGMEM rgb_color_t rosybrown                  =  0xBC8F8F;
  const PROGMEM rgb_color_t royalblue                  =  0x041690;
  const PROGMEM rgb_color_t saddlebrown                =  0x8B4513;
  const PROGMEM rgb_color_t salmon                     =  0xFA8072;
  const PROGMEM rgb_color_t sandybrown                 =  0xF4A460;
  const PROGMEM rgb_color_t seagreen                   =  0x2E8B57;
  const PROGMEM rgb_color_t seashell                   =  0xFFF5EE;
  const PROGMEM rgb_color_t sienna                     =  0xA0522D;
  const PROGMEM rgb_color_t silver                     =  0xC0C0C0;
  const PROGMEM rgb_color_t skyblue                    =  0x87CEEB;
  const PROGMEM rgb_color_t slateblue                  =  0x6A5ACD;
  const PROGMEM rgb_color_t slategray                  =  0x708090;
  const PROGMEM rgb_color_t snow                       =  0xFFFAFA;
  const PROGMEM rgb_color_t springgreen                =  0x00FF7F;
  const PROGMEM rgb_color_t steelblue                  =  0x4682B4;
  const PROGMEM rgb_color_t tan                        =  0xD2B48C;
  const PROGMEM rgb_color_t teal                       =  0x008080;
  const PROGMEM rgb_color_t thistle                    =  0xD8BFD8;
  const PROGMEM rgb_color_t tomato                     =  0xFF6347;
  const PROGMEM rgb_color_t turquoise                  =  0x40E0D0;
  const PROGMEM rgb_color_t violet                     =  0xEE82EE;
  const PROGMEM rgb_color_t wheat                      =  0xF5DEB3;
  const PROGMEM rgb_color_t white                      =  0xFFFFFF;
  const PROGMEM rgb_color_t whitesmoke                 =  0xF5F5F5;
  const PROGMEM rgb_color_t yellow                     =  0xFFFF00;
  const PROGMEM rgb_color_t yellowgreen                =  0x9ACD32;
}

#endif /* LOAD_COLOR_NAMES */


#endif /* namespace ColorNames */

