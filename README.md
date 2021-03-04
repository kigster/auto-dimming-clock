Auto-Dimming Clock with Seven-Segment Digital Display
=========

This is a repo for a series of related clocks I made for myself and friends.  They range from mid-sized bed clocks to wall clocks, some have
RGB lights, some do not, etc. 

Typically clocks are controlled with a rotary encoder knob (the type that also has a button).

This clock has been build with Teensy, Arduino Nano and Pro Micro.

Overview
=============

The clock is equipped with:

* light sensor (beneath the rotary knob) which automatically adjust brightness by up to 50% in either direction, depending on the light around (it is attached to a 10K resistor).
* rotary knob with a built-in button functionality
* 3 "neo pixel" RGB LEDs (optional)
* Adafruit 7-segment I2C display for the time display

Additional features include:
* WiFi card
* Humidity/Temperature sensor
* Potentiometer for adjusting the range of affect of the photo-sensor.

The enclosure you see on the picture is made from 1/8" acrylic using matte black color, and semi-transparent black for the front panel.

![Clock Front View](images/clock_front.jpg)
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fkigster%2Fauto-dimming-clock.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2Fkigster%2Fauto-dimming-clock?ref=badge_shield)

Functionality
=============

When the switch turns on, the clock is on.  The time is stored on Teensy backed by a 3.3V battery, and uses a crystal soldered on the bottom of Teensy.

When the clock is running, you can:
* Press single click rotary knob turns on/off neo pixel strip
* Press double click rotary knob turns off/on 7-segment display
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

![Clock Top View](images/clock_top.jpg)

Hardware
========

If there is interest, I can describe in more detail how the hardware is put together, so please create an issue on github to register your interest.

For the time being, here is a high zoom shot into the land of wires and neatly arranged pieces :)

![Clock Inside View](images/clock_inside.jpg)


Author
======

(c) 2015 Konstantin Gredeskoul, https://github.com/kigster

Distributed under MIT license, see [LICENSE](LICENSE).


## License
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fkigster%2Fauto-dimming-clock.svg?type=large)](https://app.fossa.com/projects/git%2Bgithub.com%2Fkigster%2Fauto-dimming-clock?ref=badge_large)