WallClock
=========

Teensy-based digital clock with rotary encoder button used to set time, and neo-pixels for the fun of it :)

Yes, teensy is probably an overkill for this project, but it does provide some interesting options in the future.

Overview
=============

The clock is equipped with:

* light sensor (beneath the rotary knob)
* rotary knob with a built-in button functionality
* neo pixels
* Adafruit 7-segment I2C display

![Clock Front View](images/clock_front.jpg)

Functionality
=============

When the switch turns on, the clock is on.  The time is stored on Teensy backed by a 3.3V battery, and uses a crystal soldered on the bottom of Teensy.

When the clock is running, you can:
* single click rotary knob turns on/off neo pixel strip
* double click rotary knob turns off/on 7-segment display
* moving rotary knob in this mode changes the brightness. There are 16 total values for brightness, from 0 to 15.
* press and hold enters the menu to set the time
  * Once in the menu setup, rotating the knob changes the current hour.  When done, click the knob once.
	  * If during either hour or minutes setup you rather cancel this process, double click the button.
  * Now change the minutes, and when ready click the knob once.
  * Now the time will be set and saved, and configuration mode is finished.
    
Libraries
=========
Supplied with Teensy:
* Wire
* DS1307RTC
* Adafruit_GFX
* Time

Additional dependencies:
* [LiquidCrystal_I2C](https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library) (only needed for a debug LCD 4x20 screen, which can be disabled with a macro define)
* [Adafruit_LEDBackpack](https://github.com/adafruit/Adafruit-LED-Backpack-Library)
* [RotaryEncoderWithButton](https://github.com/kigster/kiguino/tree/master/arduino/libraries/RotaryEncoderWithButton)
* [OneButton](https://github.com/mathertel/OneButton)

![Clock Top View](images/clock_top_.jpg)


Author
======

(c) 2015 Konstantin Gredeskoul, https://github.com/kigster

Distributed under MIT license, see [LICENSE](LICENSE).
