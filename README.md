# PS2KeyAdvanced
## Arduino PS2 Keyboard FULL keyboard protocol support and full keys to integer coding
**V1.0.8** November 2020 - Add support for STM32 from user Hiabuto-de
                  Tested on STM32Duino-Framework and PlatformIO on STM32F103C8T6 and an IBM Model M

V1.0.7 March 2020 - Add notes for porting to other platforms, tidy ups, add SAMD1

V1.0.6 January 2020 - Fix typos, correct keyboard reset status improve library.properties 
		   and additional reduction for easier platform handling

V1.0.4 August 2018 - Minor reduction in available() method to remove redundant extra safety checks on buffers

V1.0.3 July 2018 Improved keywords file

First Public Release Version V1.0.2

Smaller footprint than any others found and more functionality.

For other versions that just read the keycodes for all keyboard types or allow you to get the full UTF-8 configurable for any LATIN keyboard with extra functions keys as simple codes see associated reporitories for PS2KeyAdvanced and PS2KeyMap libraries. Yes these libraries even provide codes for multimedia keys and Function keys F1 to F24 (yes that is F24).

### Platforms

-  Arduino AVR - tested on Uno and Mega 2560 (Users have tested on Teensy 2.0 and Teensy++ 2.0)
-  Arduino SAM - tested on DUE
-  SAMD1 - Seeeduino Xiao M0+ (tested by user)
-  STM32 - STM32Duino-Framework and PlatformIO on STM32F103C8T6 from user Hiabuto-de

See later for porting to other platforms

### Test Environment

    All Arduino platforms versions of test R3 Hardware
    Arduino V1.6.7 and newer
    DUE Board Manager package V1.6.6 or higher

## Important Hardware Notes

1. Make sure you have data and clock wires connected correctly. Clock wire MUST be connected to an interrupt pin.

  -  Do NOT use Pin 13 for either clock or data wires
  -  PS2KeyAdvanced requires both pins specified for begin() as in keyboard.begin( data_pin, irq_pin );
  -  Valid irq pins:
      -  Arduino Uno: 2, 3
      -  Arduino Due: All pins, except 13 (LED)
      -  Arduino Mega: 2, 3, 18, 19, 20, 21
      -  Teensy 2.0: All pins, except 13 (LED)
      -  Teensy 2.0: 5, 6, 7, 8
      -  Teensy 1.0: 0, 1, 2, 3, 4, 6, 7, 16
      -  Teensy++ 2.0: 0, 1, 2, 3, 18, 19, 36, 37
      -  Teensy++ 1.0: 0, 1, 2, 3, 18, 19, 36, 37
      -  Sanguino: 2, 10, 11

2. When using DUE or other boards with 3V3 I/O you MUST use a level translator FET or IC like Texas Instruments TXS0102 or similar as most keyboards not only operate at 5V but the two wire communications are pulled up by a resistor to 5V at the keyboard end.

### Introduction
After looking round for suitable libraries I found most were lacking in functionality and high in code and data footprint, so I created a series of PS2 Keyboard libraries. This is the second which fully supports the PS2 Keyboard Protocol, even allowing you control of keyboard LEDs (some have 4 LEDs) and changing settings..

The PS2 Keyboard interface is still needed for systems that have no USB and even if you have USB, you want it left for other uses.

The PS2 Keyboard interface is a Bi-directional two wire interface with a clock line and a data line which you connect to your Arduino (see above), the keyboard protocol has many nuances all of which are used in the other libraries of this series. this library allows you to access the keycodes sent from a keyboard into its small buffer and read out the codes with simple methods.

Returns any keypress as 16 bit integer, which includes a coded value for the key along with status for

   - Make/Break
   - CTRL, SHIFT, CAPS, ALT, GUI, ALT-GR Status
   - Alphanumeric/keyboard Function
   - 8 bit key code (defined in public header)

Fully featured PS2 keyboard library to provide

    - All keys have a keycode (ESC, A-Z and 0-9 as ASCII equivalents)
    - All function (F1 to F24), multimedia and movement keys supported
    - Parity checking of data sent/received
    - Resends data and requests resends when needed
    - Functions for get and/or set of
        - Scancode set in use
        - LED and LOCK control
        - ReadID
        - Reset keyboard
        - Send ECHO
    - Ignore Break codes for keys
    - Ignore typematic repeat of CTRL, SHIFT, ALT, Num, Scroll, Caps
    - Handles NUM, CAPS and SCROLL lock keys to LEDs
    - Handles NUM/SCROLL internally

### Installation
Performed by standard zip file library inclusion into Arduino IDE
### Examples
This library has THREE examples, from simplest to most complec -

  - SimpleTest that uses the serial port to output the converted codes received on every keystroke and auto-repeat.
  - advcodetest that uses serial port and some of the keys pressed to send commands back to the keyboard to see the responses for things like Reset keyboard, Read ID, change Typematic rate (auto-repeat).
  - KeyToLCD - Example that will allow you to display keyboard actions on LCD connected to Arduino and allow cursor movements to move the cursor on LCD, whilst also displaying strings for keys like ESC, TAB, F1 to F12
  
## Porting to different boards or architectures
See document Porting.md in extras folder for hints to assist in this process
### Contributor and Author Details
Author Paul Carpenter, PC Services

Web Site http://www.pcserviceselectronics.co.uk
