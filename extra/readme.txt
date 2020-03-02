  PS2KeyAdvanced - PS2KeyAdvanced library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Paul Carpenter, PC Services <sales@pcserviceselectronics.co.uk>
  Created September 2014
  Updated January 2016
  Updated February 2020

  This is for a LATIN style keyboard.

  Assumption ONLY ONE keyboard connected to system

  Fully featured PS2 keyboard library to provide
    All keys have a keycode (ESC, A-Z and 0-9 as ASCII equivalents)
    All function and movement keys supported
    Parity checking of data sent/received
    Resends data and requests resends when needed
    Functions for get and/or set of
        Scancode set in use
        LED and LOCK control
        ReadID
        Reset keyboard
        Send ECHO
    Ignore Break codes for keys
        Ignore typematic repeat of CTRL, SHIFT, ALT, Num, Scroll, Caps
        Handles NUM, CAPS and SCROLL lock keys to LEDs
        Handles NUM/SCROLL internally

  Makes it easier to catch key combinations like press of <CTRL> + <ENTER> or 
  <CTRL> + <ALT> + <DEL>.

  You can even detect repetitions or not of keys or even when a key is released.

  See extras\codes.txt for useful defines available in the library for key codes
  and combinations. Note often codes like oxFA are received from the keyboard 
  after commands are sent this code is the ACKNOWLEDGE data from the keyboard
  to say received correctly.

  See also file extras\websites.txt for website information about PS2 interface, 
  protocol scancodes and UTF-8 encoding.

  Files are

   extras folder
    codes.txt       The literals (constants) you can use to match codes returned
    PS2 Keyboard.pdf Copy of Website explaining PS2 Protocol
    readme.txt      this file
    websites.txt    Other websites about PS2 keyboard and UTF-8 character encoding
    Images          folder containing images for a single NUMLOCK keypress and what
                    happens on the wires.

   src folder
    PS2KeyAdvanced.cpp   the library code
    PS2KeyAdvanced.h     Library Header for sketches defines class and values returned for keys
    PS2KeyCode.h         internal header file for codes from actual keyboard
    PS2KeyTable.h        internal table of translations from keyboard to return code

   examples folder
    SimpleTest      reads every returned keycode back to serial (to get codes for mapping)
    advcodetest     reads keyboard and sends codes in hex to serial
    KeyToLCD        reads keyboard and displays where possible on LCD

  A full protocol handshake for the sequence of pressing the NUMLOCK key and 
  then turning on the NUMLOCK LED is shown as a set of scope screenshots in
  extras\Images . These are

    NUMLock-sequence.PNG  The complete sequence (both ways)

  In detail the other images highlight parts of the sequence -

    NUMLock-77.png     keyboard sends NUMLOCK byte (0x77)

    NUMLock-ED-FA.png  Arduino sends light LEDs command (0xED) 
               keyboard responds ACKNOWLEDGE (0xFA)

    NUMLock-02-FA.png  Arduino sends data for LED combination (0x02)
               keyboard responds ACKNOWLEDGE (0xFA)

  Reading a key code returns an UNSIGNED INT containing
        Make/Break status
        Caps status
        Shift, Ctrl, Alt, Alt Gr, GUI keys
        Flag for function key not a displayable/printable character
        8 bit key code

  To get the key as ASCII/UTF-8 single byte character conversion requires use
  of PS2KeyMap library AS WELL.

  Code Ranges (bottom byte of unsigned int)
        0       invalid/error
        1-1F    Functions (caps, shift, alt, enter, del... )
        1A-1F   Functions with ASCII control code (DEL, BS, TAB, ESC, ENTER, SPACE)
        20-60   Printable characters noting
                    0-9 = 0x30 to 0x39 as ASCII
                    A to Z = 0x41 to 0x5A as upper case ASCII type codes
        61-9F   Function keys and other special keys (plus F2 and F1)
                    61-78 F1 to F24
                    79-8A Multimedia
                    8B-8D ACPI power
                    90-9F Special multilingual
        A0-FF   Keyboard communications commands (note F2 and F1 are special
                codes for special multi-lingual keyboards)

    By using these ranges it is possible to perform detection of any key and do
    easy translation to ASCII/UTF-8 avoiding keys that do not have a valid code.
    See code defines below for how to decode top byte.

    Top Byte is 8 bits denoting as follows and standard defines available -

    Define name bit     description
    PS2_BREAK   15      1 = Break key code
               (MSB)    0 = Make Key code
    PS2_SHIFT   14      1 = Shift key pressed as well (either)
                        0 = NO shift key
    PS2_CTRL    13      1 = Ctrl key pressed as well (either)
                        0 = NO Ctrl key
    PS2_CAPS    12      1 = Caps Lock ON
                        0 = Caps lock OFF
    PS2_ALT     11      1 = Left Alt key pressed as well
                        0 = NO Left Alt key
    PS2_ALT_GR  10      1 = Right Alt (Alt GR) key pressed as well
                        0 = NO Right Alt key
    PS2_GUI      9      1 = GUI key pressed as well (either)
                        0 = NO GUI key
    PS2_FUNCTION 8      1 = FUNCTION key non-printable character (plus space, tab, enter)
                        0 = standard character key

  Error Codes
     Most functions return 0 or 0xFFFF as error, other codes to note and
     handle appropriately, which are responses from the keyboard -

        0xAA   keyboard has reset and passed power up tests
               will happen if keyboard plugged in after code start
        0xFC   keyboard failed power up tests

  It is responsibility of your programme to deal with converting special cases
  like <CTRL>+<ENTER> sends a special value to something else.

  See extras\codes.txt for codes from the keyboard this library uses to decode.
  These are defines so that you can use a standard name for ANY key without
  knowing the actual value.

  Returned Keycodes in lower half of unsigned integer returned are detailed in
  extras\codes.txt

 Parameters for other functions are
 ===================================

 For Typematic Rate (repeat rate and delay to repeat), setting two parameters
    first value 0 to 0x1F as per table below
    second value defines listed below

  Typematic (repeat) Rate use codes in Rate column and (repeat) delay defines
    Rate    CPS     Rate    CPS     Rate    CPS     Rate    CPS
    ------------------------------------------------------------
    0       30      8       15      10      7.5     18      3.7
    1       26.7    9       13.3    11      6.7     19      3.3
    2       24      A       12      12      6       1A      3
    3       21.8    B       10.9    13      5.5     1B      2.7
    4       20.7    C       10      14      5       1C      2.5
    5       18.5    D       9.2     15      4.6     1D      2.3
    6       17.1    E       8.6     16      4.3     1E      2.1
    7       16      F       8       17      4       1F      2

    Delay           define
    --------------------------
    0.25 seconds    PS2_DELAY0
    0.5 seconds     PS2_DELAY1
    0.75 seconds    PS2_DELAY2
    1 second        PS2_DELAY3

For LEDs command use the following defines added together to set them
    PS2_LOCK_SCROLL
    PS2_LOCK_NUM
    PS2_LOCK_CAPS
    PS2_LOCK_EXTRA (for keyboards with 4th LED)


Paul Carpenter
PC Services
February 2020
http://www.pcserviceselectronics.co.uk