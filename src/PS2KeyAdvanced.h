/* Version V1.0.9
  PS2KeyAdvanced.h - PS2KeyAdvanced library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Paul Carpenter, PC Services <sales@pcserviceselectronics.co.uk>
  Created September 2014
  Updated January 2016 - Paul Carpenter - add tested on Due and tidy ups for V1.5 Library Management
    January 2020   Fix typos, correct keyboard reset status improve library.properties 
		   and additional platform handling and some documentation
    March 2020  Add SAMD1 as recognised support as has been tested by user
                Improve different architecture handling
    November 2020 Add support for STM32 from user Hiabuto-de
                  Tested on STM32Duino-Framework and PlatformIO on STM32F103C8T6 and an IBM Model M
    July 2021   Add workaround for ESP32 issue with Silicon (hardware) from user submissions

  IMPORTANT WARNING
 
    If using a DUE or similar board with 3V3 I/O you MUST put a level translator 
    like a Texas Instruments TXS0102 or FET circuit as the signals are 
    Bi-directional (signals transmitted from both ends on same wire).
 
    Failure to do so may damage your Arduino Due or similar board.

  Test History
    September 2014 Uno and Mega 2560 September 2014 using Arduino V1.6.0
    January 2016   Uno, Mega 2560 and Due using Arduino 1.6.7 and Due Board 
                    Manager V1.6.6

  ONLY use defines in this file others may disappear on updates.

  This is for a LATIN style keyboard using Scan code set 2. See various
  websites on what different scan code sets use. Scan Code Set 2 is the
  default scan code set for PS2 keyboards on power up.

  Will support most keyboards even ones with multimedia keys or even 24 function keys.

  Fully featured PS2 keyboard library to provide
    All function and movement keys supported even multi-lingual
    Parity checking of data sent/received on receive request keyboard resend
    Resends data when needed handles keyboard protocol for RESEND and ECHO
    Functions for get and set of
        Scancode set in use READ only
        LED and LOCK control
        ReadID
        Reset keyboard
        Send ECHO
	    Ignore Break codes for keys
        Ignore typematic repeat of CTRL, SHIFT, ALT, Num, Scroll, Caps
        Handles NUM, CAPS and SCROLL lock keys to LEDs
        Handles NUM/SCROLL internally

  Read function Returns an UNSIGNED INT containing
        Make/Break status
        Caps status
        Shift, CTRL, ALT, ALT GR, GUI keys
        Flag for function key not a displayable/printable character
        8 bit key code

  Code Ranges (bottom byte of unsigned int)
        0       invalid/error
        1-1F    Functions (Caps, Shift, ALT, Enter, DEL... )
        1A-1F   Functions with ASCII control code
                    (DEL, BS, TAB, ESC, ENTER, SPACE)
        20-61   Printable characters noting
                    0-9 = 0x30 to 0x39 as ASCII
                    A to Z = 0x41 to 0x5A as upper case ASCII type codes
                    8B Extra European key
        61-A0   Function keys and other special keys (plus F2 and F1)
                    61-78 F1 to F24
                    79-8A Multimedia
                    8B NOT included
                    8C-8E ACPI power
                    91-A0 and F2 and F1 - Special multilingual
        A8-FF   Keyboard communications commands (note F2 and F1 are special
                codes for special multi-lingual keyboards)

    By using these ranges it is possible to perform detection of any key and do
    easy translation to ASCII/UTF-8 avoiding keys that do not have a valid code.

    Top Byte is 8 bits denoting as follows with defines for bit code

        Define name bit     description
        PS2_BREAK   15      1 = Break key code
                   (MSB)    0 = Make Key code
        PS2_SHIFT   14      1 = Shift key pressed as well (either side)
                            0 = NO shift key
        PS2_CTRL    13      1 = Ctrl key pressed as well (either side)
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
     handle appropriately
        0xAA   keyboard has reset and passed power up tests
               will happen if keyboard plugged in after code start
        0xFC   Keyboard General error or power up fail

  It is responsibility of your programme to deal with converting special cases like
  <CTRL>+<ENTER> sends a special code to something else. If you wish to do that make a
  NEW library called SOMETHING different NOT a variant or revision of this one, as you
  are changing base functionality

  See PS2KeyCode.h for codes from the keyboard this library uses to decode.
  (may disappear in updates do not rely on that file or definitions)

  See this file for returned definitions of Keys

  Note defines starting
            PS2_KC_*  are internal defines for codes from the keyboard
            PS2_KEY_* are the codes this library returns
            PS2_*     remaining defines for use in higher levels

  To get the key as ASCII/UTF-8 single byte character conversion requires use
  of PS2KeyMap library AS WELL.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef PS2KeyAdvanced_h
#define PS2KeyAdvanced_h

// Platform specific areas
// Harvard architecture settings for PROGMEM
// Add separate for EACH architecture as easier to maintain
// AVR (includes Teensy 2.0)
#if defined( ARDUINO_ARCH_AVR )
#define PS2_SUPPORTED           1
#define PS2_REQUIRES_PROGMEM    1
#define PS2_CLEAR_PENDING_IRQ   1
#endif
// SAM (Due)
#if defined( ARDUINO_ARCH_SAM )
#define PS2_SUPPORTED           1
#define PS2_CLEAR_PENDING_IRQ   1
#endif
// SAMD1
#if defined( ARDUINO_ARCH_SAMD1 )
#define PS2_SUPPORTED           1
#define PS2_CLEAR_PENDING_IRQ   1
#endif
// STM32
#if defined( ARDUINO_ARCH_STM32 )
#define PS2_SUPPORTED           1
#define PS2_CLEAR_PENDING_IRQ   1
#endif
// ESP32
#if defined( ARDUINO_ARCH_ESP32 )
#define PS2_SUPPORTED           1
#define PS2_ONLY_CHANGE_IRQ     1
#endif

// Invalid architecture
#if !( defined( PS2_SUPPORTED ) )
#warning Library is NOT supported on this board Use at your OWN risk
#endif

/* Flags/bit masks for status bits in returned unsigned int value */
#define PS2_BREAK   0x8000
#define PS2_SHIFT   0x4000
#define PS2_CTRL    0x2000
#define PS2_CAPS    0x1000
#define PS2_ALT      0x800
#define PS2_ALT_GR   0x400
#define PS2_GUI      0x200
#define PS2_FUNCTION 0x100

/* General defines of communications codes */
/* Command or response */
#define PS2_KEY_RESEND   0xFE
#define PS2_KEY_ACK      0xFA
#define PS2_KEY_ECHO     0xEE
/* Responses */
#define PS2_KEY_BAT      0xAA
// Actually buffer overrun
#define PS2_KEY_OVERRUN  0xFF
// Below is general error code
#define PS2_KEY_ERROR    0xFC

/* Command parameters for functions */
/* LED codes OR together */
#define PS2_LOCK_SCROLL  0x01
#define PS2_LOCK_NUM     0x02
#define PS2_LOCK_CAPS    0x04
/* Only useful for very few keyboards */
#define PS2_LOCK_EXTRA   0x08

/* Returned keycode definitions */
/* Do NOT change these codings as you will break base
   functionality use PS2KeyMap for that and internationalisation */
#define PS2_KEY_NUM         0x01
#define PS2_KEY_SCROLL      0x02
#define PS2_KEY_CAPS        0x03
#define PS2_KEY_PRTSCR      0x04
#define PS2_KEY_PAUSE       0x05
#define PS2_KEY_L_SHIFT     0x06
#define PS2_KEY_R_SHIFT     0x07
#define PS2_KEY_L_CTRL      0X08
#define PS2_KEY_R_CTRL      0X09
#define PS2_KEY_L_ALT       0x0A
#define PS2_KEY_R_ALT       0x0B
/* Sometimes called windows key */
#define PS2_KEY_L_GUI       0x0C
#define PS2_KEY_R_GUI       0x0D
#define PS2_KEY_MENU        0x0E
/* Break is CTRL + PAUSE generated inside keyboard */
#define PS2_KEY_BREAK       0x0F
/* Generated by some keyboards by ALT and PRTSCR */
#define PS2_KEY_SYSRQ       0x10
#define PS2_KEY_HOME        0x11
#define PS2_KEY_END         0x12
#define PS2_KEY_PGUP        0x13
#define PS2_KEY_PGDN        0x14
#define PS2_KEY_L_ARROW     0x15
#define PS2_KEY_R_ARROW     0x16
#define PS2_KEY_UP_ARROW    0x17
#define PS2_KEY_DN_ARROW    0x18
#define PS2_KEY_INSERT      0x19
#define PS2_KEY_DELETE      0x1A
#define PS2_KEY_ESC         0x1B
#define PS2_KEY_BS          0x1C
#define PS2_KEY_TAB         0x1D
#define PS2_KEY_ENTER       0x1E
#define PS2_KEY_SPACE       0x1F
#define PS2_KEY_KP0         0x20
#define PS2_KEY_KP1         0x21
#define PS2_KEY_KP2         0x22
#define PS2_KEY_KP3         0x23
#define PS2_KEY_KP4         0x24
#define PS2_KEY_KP5         0x25
#define PS2_KEY_KP6         0x26
#define PS2_KEY_KP7         0x27
#define PS2_KEY_KP8         0x28
#define PS2_KEY_KP9         0x29
#define PS2_KEY_KP_DOT      0x2A
#define PS2_KEY_KP_ENTER    0x2B
#define PS2_KEY_KP_PLUS     0x2C
#define PS2_KEY_KP_MINUS    0x2D
#define PS2_KEY_KP_TIMES    0x2E
#define PS2_KEY_KP_DIV      0x2F
#define PS2_KEY_0           0X30
#define PS2_KEY_1           0X31
#define PS2_KEY_2           0X32
#define PS2_KEY_3           0X33
#define PS2_KEY_4           0X34
#define PS2_KEY_5           0X35
#define PS2_KEY_6           0X36
#define PS2_KEY_7           0X37
#define PS2_KEY_8           0X38
#define PS2_KEY_9           0X39
#define PS2_KEY_APOS        0X3A
#define PS2_KEY_COMMA       0X3B
#define PS2_KEY_MINUS       0X3C
#define PS2_KEY_DOT         0X3D
#define PS2_KEY_DIV         0X3E
/* Some Numeric keyboards have an '=' on right keypad */
#define PS2_KEY_KP_EQUAL    0x3F
/* Single quote or back quote */
#define PS2_KEY_SINGLE      0X40
#define PS2_KEY_A           0X41
#define PS2_KEY_B           0X42
#define PS2_KEY_C           0X43
#define PS2_KEY_D           0X44
#define PS2_KEY_E           0X45
#define PS2_KEY_F           0X46
#define PS2_KEY_G           0X47
#define PS2_KEY_H           0X48
#define PS2_KEY_I           0X49
#define PS2_KEY_J           0X4A
#define PS2_KEY_K           0X4B
#define PS2_KEY_L           0X4C
#define PS2_KEY_M           0X4D
#define PS2_KEY_N           0X4E
#define PS2_KEY_O           0X4F
#define PS2_KEY_P           0X50
#define PS2_KEY_Q           0X51
#define PS2_KEY_R           0X52
#define PS2_KEY_S           0X53
#define PS2_KEY_T           0X54
#define PS2_KEY_U           0X55
#define PS2_KEY_V           0X56
#define PS2_KEY_W           0X57
#define PS2_KEY_X           0X58
#define PS2_KEY_Y           0X59
#define PS2_KEY_Z           0X5A
#define PS2_KEY_SEMI        0X5B
#define PS2_KEY_BACK        0X5C
#define PS2_KEY_OPEN_SQ     0X5D
#define PS2_KEY_CLOSE_SQ    0X5E
#define PS2_KEY_EQUAL       0X5F
/* Some Numeric keypads have a comma key */
#define PS2_KEY_KP_COMMA    0x60
#define PS2_KEY_F1          0X61
#define PS2_KEY_F2          0X62
#define PS2_KEY_F3          0X63
#define PS2_KEY_F4          0X64
#define PS2_KEY_F5          0X65
#define PS2_KEY_F6          0X66
#define PS2_KEY_F7          0X67
#define PS2_KEY_F8          0X68
#define PS2_KEY_F9          0X69
#define PS2_KEY_F10         0X6A
#define PS2_KEY_F11         0X6B
#define PS2_KEY_F12         0X6C
#define PS2_KEY_F13         0X6D
#define PS2_KEY_F14         0X6E
#define PS2_KEY_F15         0X6F
#define PS2_KEY_F16         0X70
#define PS2_KEY_F17         0X71
#define PS2_KEY_F18         0X72
#define PS2_KEY_F19         0X73
#define PS2_KEY_F20         0X74
#define PS2_KEY_F21         0X75
#define PS2_KEY_F22         0X76
#define PS2_KEY_F23         0X77
#define PS2_KEY_F24         0X78
#define PS2_KEY_NEXT_TR     0X79
#define PS2_KEY_PREV_TR     0X7A
#define PS2_KEY_STOP        0X7B
#define PS2_KEY_PLAY        0X7C
#define PS2_KEY_MUTE        0X7D
#define PS2_KEY_VOL_UP      0X7E
#define PS2_KEY_VOL_DN      0X7F
#define PS2_KEY_MEDIA       0X80
#define PS2_KEY_EMAIL       0X81
#define PS2_KEY_CALC        0X82
#define PS2_KEY_COMPUTER    0X83
#define PS2_KEY_WEB_SEARCH  0X84
#define PS2_KEY_WEB_HOME    0X85
#define PS2_KEY_WEB_BACK    0X86
#define PS2_KEY_WEB_FORWARD 0X87
#define PS2_KEY_WEB_STOP    0X88
#define PS2_KEY_WEB_REFRESH 0X89
#define PS2_KEY_WEB_FAVOR   0X8A
#define PS2_KEY_EUROPE2     0X8B
#define PS2_KEY_POWER       0X8C
#define PS2_KEY_SLEEP       0X8D
#define PS2_KEY_WAKE        0X90
#define PS2_KEY_INTL1       0X91
#define PS2_KEY_INTL2       0X92
#define PS2_KEY_INTL3       0X93
#define PS2_KEY_INTL4       0X94
#define PS2_KEY_INTL5       0X95
#define PS2_KEY_LANG1       0X96
#define PS2_KEY_LANG2       0X97
#define PS2_KEY_LANG3       0X98
#define PS2_KEY_LANG4       0X99
#define PS2_KEY_LANG5       0xA0

/*
  Purpose: Provides advanced access to PS2 keyboards
  Public class definitions

  See standard error codes for error code returns
 */
class PS2KeyAdvanced {
  public:
  	/* This constructor does basically nothing. Please call the begin(int,int)
  	   method before using any other method of this class. 	 */
    PS2KeyAdvanced( );

    /* Starts the keyboard "service" by registering the external interrupt.
       setting the pin modes correctly and driving those needed to high.
       Sets default LOCK status (LEDs) to passed in value or default of all off
       The best place to call this method is in the setup routine.    */
    void begin( uint8_t, uint8_t );

    /* Returns number of codes available or 0 for none */
    uint8_t available( );

    /* Returns the key last read from the keyboard.
       If there is no key available, 0 is returned.  */
    uint16_t read( );

    /* Returns the current status of Locks
        Use Macro to mask out bits from
        PS2_LOCK_NUM    PS2_LOCK_CAPS   PS2_LOCK_SCROLL */
    uint8_t getLock( );

    /* Sets the current status of Locks and LEDs
       Use macro defines added together from
        PS2_LOCK_NUM    PS2_LOCK_CAPS   PS2_LOCK_SCROLL */
    void setLock( byte );

    /* Set library to not send break key codes
            1 = no break codes
            0 = send break codes  */
    void setNoBreak( uint8_t );

    /* Set library to not repeat make codes for CTRL, ALT, GUI, SHIFT
            1 = no repeat codes
            0 = send repeat codes  */
    void setNoRepeat( uint8_t );

    /* Resets keyboard when reset has completed
       keyboard sends AA - Pass or FC for fail
       Read from keyboard data buffer */
    void resetKey( );

    /*  Get the current Scancode Set used in keyboard
        returned data in keyboard buffer read as keys */
    void getScanCodeSet( void );

    /*  Get the current Scancode Set used in keyboard
        returned data in keyboard buffer read as keys */
    void readID( void );

    /*  Send Echo command to keyboard
        returned data in keyboard buffer read as keys */
    void echo( void );

    /*  Send Typematic rate/delay command to keyboard
       First Parameter  rate is 0 - 0x1F (31)
                0 = 30 CPS
                0x1F = 2 CPS
                default in keyboard is 0xB (10.9 CPS)
       Second Parameter delay is 0 - 3 for 0.25s to 1s in 0.25 increments
         default in keyboard is 1 = 0.5 second delay
        Returned data in keyboard buffer read as keys */
    int typematic( uint8_t , uint8_t );
};
#endif
