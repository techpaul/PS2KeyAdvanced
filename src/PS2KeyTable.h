/* Version V1.0.8
  PS2KeyTable.h - PS2KeyAdvanced library keycode values to return values
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Paul Carpenter, PC Services <sales@pcserviceselectronics.co.uk>
  Created September 2014
  V1.0.2 Updated January 2016 - Paul Carpenter - add tested on Due and tidy ups for V1.5 Library Management
  
  PRIVATE to library

  Test History
    September 2014 Uno and Mega 2560 September 2014 using Arduino V1.6.0
    January 2016   Uno, Mega 2560 and Due using Arduino 1.6.7 and Due Board 
                    Manager V1.6.6

  Internal to library private tables
  (may disappear in updates do not rely on this file or definitions)

  This is for a LATIN style keyboard. Will support most keyboards even ones
  with multimedia keys or even 24 function keys.

  Definitions used for key codes from a PS2 keyboard, do not use in your
  code these are handled by the library.

  See PS2KeyAdvanced.h for codes returned from library and flag settings

  Two sets of tables

     Single Byte codes returned as key codes

     Two byte Codes preceded by E0 code returned as keycodes

  Same tables used for make and break decode

  Special cases are -

    PRTSCR that ignores one of the sequences (E0,12) as this is not always sent
    especially with modifier keys or some keyboards when typematic repeat comes on.

    PAUSE as this is an 8 byte sequence only one starting E1 so main code gets E1
    and waits for 7 more valid bytes to make the coding.

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
#ifndef PS2KeyTable_h
#define PS2KeyTable_h

/* Table contents are pairs of numbers
    first code from keyboard
    second is either PS2_KEY_IGNOPRE code or key code to return

   Single byte Key table
    In codes can only be 1 - 0x9F, plus 0xF2 and 0xF1
    Out Codes in range 1 to 0x9F
*/
#if defined(PS2_REQUIRES_PROGMEM)
const uint8_t PROGMEM single_key[][ 2 ] = {
#else
const uint8_t single_key[][ 2 ] = {
#endif
                { PS2_KC_NUM, PS2_KEY_NUM },
                { PS2_KC_SCROLL, PS2_KEY_SCROLL },
                { PS2_KC_CAPS, PS2_KEY_CAPS },
                { PS2_KC_L_SHIFT, PS2_KEY_L_SHIFT },
                { PS2_KC_R_SHIFT, PS2_KEY_R_SHIFT },
                { PS2_KC_CTRL, PS2_KEY_L_CTRL },
                { PS2_KC_ALT, PS2_KEY_L_ALT },
                { PS2_KC_SYSRQ, PS2_KEY_SYSRQ },
                { PS2_KC_ESC, PS2_KEY_ESC },
                { PS2_KC_BS, PS2_KEY_BS },
                { PS2_KC_TAB, PS2_KEY_TAB },
                { PS2_KC_ENTER, PS2_KEY_ENTER },
                { PS2_KC_SPACE, PS2_KEY_SPACE },
                { PS2_KC_KP0, PS2_KEY_KP0 },
                { PS2_KC_KP1, PS2_KEY_KP1 },
                { PS2_KC_KP2, PS2_KEY_KP2 },
                { PS2_KC_KP3, PS2_KEY_KP3 },
                { PS2_KC_KP4, PS2_KEY_KP4 },
                { PS2_KC_KP5, PS2_KEY_KP5 },
                { PS2_KC_KP6, PS2_KEY_KP6 },
                { PS2_KC_KP7, PS2_KEY_KP7 },
                { PS2_KC_KP8, PS2_KEY_KP8 },
                { PS2_KC_KP9, PS2_KEY_KP9 },
                { PS2_KC_KP_DOT, PS2_KEY_KP_DOT },
                { PS2_KC_KP_PLUS, PS2_KEY_KP_PLUS },
                { PS2_KC_KP_MINUS, PS2_KEY_KP_MINUS },
                { PS2_KC_KP_TIMES, PS2_KEY_KP_TIMES },
                { PS2_KC_KP_EQUAL, PS2_KEY_KP_EQUAL },
                { PS2_KC_0, PS2_KEY_0 },
                { PS2_KC_1, PS2_KEY_1 },
                { PS2_KC_2, PS2_KEY_2 },
                { PS2_KC_3, PS2_KEY_3 },
                { PS2_KC_4, PS2_KEY_4 },
                { PS2_KC_5, PS2_KEY_5 },
                { PS2_KC_6, PS2_KEY_6 },
                { PS2_KC_7, PS2_KEY_7 },
                { PS2_KC_8, PS2_KEY_8 },
                { PS2_KC_9, PS2_KEY_9 },
                { PS2_KC_APOS, PS2_KEY_APOS },
                { PS2_KC_COMMA, PS2_KEY_COMMA },
                { PS2_KC_MINUS, PS2_KEY_MINUS },
                { PS2_KC_DOT, PS2_KEY_DOT },
                { PS2_KC_DIV, PS2_KEY_DIV },
                { PS2_KC_SINGLE, PS2_KEY_SINGLE },
                { PS2_KC_A, PS2_KEY_A },
                { PS2_KC_B, PS2_KEY_B },
                { PS2_KC_C, PS2_KEY_C },
                { PS2_KC_D, PS2_KEY_D },
                { PS2_KC_E, PS2_KEY_E },
                { PS2_KC_F, PS2_KEY_F },
                { PS2_KC_G, PS2_KEY_G },
                { PS2_KC_H, PS2_KEY_H },
                { PS2_KC_I, PS2_KEY_I },
                { PS2_KC_J, PS2_KEY_J },
                { PS2_KC_K, PS2_KEY_K },
                { PS2_KC_L, PS2_KEY_L },
                { PS2_KC_M, PS2_KEY_M },
                { PS2_KC_N, PS2_KEY_N },
                { PS2_KC_O, PS2_KEY_O },
                { PS2_KC_P, PS2_KEY_P },
                { PS2_KC_Q, PS2_KEY_Q },
                { PS2_KC_R, PS2_KEY_R },
                { PS2_KC_S, PS2_KEY_S },
                { PS2_KC_T, PS2_KEY_T },
                { PS2_KC_U, PS2_KEY_U },
                { PS2_KC_V, PS2_KEY_V },
                { PS2_KC_W, PS2_KEY_W },
                { PS2_KC_X, PS2_KEY_X },
                { PS2_KC_Y, PS2_KEY_Y },
                { PS2_KC_Z, PS2_KEY_Z },
                { PS2_KC_SEMI, PS2_KEY_SEMI },
                { PS2_KC_BACK, PS2_KEY_BACK },
                { PS2_KC_OPEN_SQ, PS2_KEY_OPEN_SQ },
                { PS2_KC_CLOSE_SQ, PS2_KEY_CLOSE_SQ },
                { PS2_KC_EQUAL, PS2_KEY_EQUAL },
                { PS2_KC_EUROPE2, PS2_KEY_EUROPE2 },
                { PS2_KC_F1, PS2_KEY_F1 },
                { PS2_KC_F2, PS2_KEY_F2 },
                { PS2_KC_F3, PS2_KEY_F3 },
                { PS2_KC_F4, PS2_KEY_F4 },
                { PS2_KC_F5, PS2_KEY_F5 },
                { PS2_KC_F6, PS2_KEY_F6 },
                { PS2_KC_F7, PS2_KEY_F7 },
                { PS2_KC_F8, PS2_KEY_F8 },
                { PS2_KC_F9, PS2_KEY_F9 },
                { PS2_KC_F10, PS2_KEY_F10 },
                { PS2_KC_F11, PS2_KEY_F11 },
                { PS2_KC_F12, PS2_KEY_F12 },
                { PS2_KC_F13, PS2_KEY_F13 },
                { PS2_KC_F14, PS2_KEY_F14 },
                { PS2_KC_F15, PS2_KEY_F15 },
                { PS2_KC_F16, PS2_KEY_F16 },
                { PS2_KC_F17, PS2_KEY_F17 },
                { PS2_KC_F18, PS2_KEY_F18 },
                { PS2_KC_F19, PS2_KEY_F19 },
                { PS2_KC_F20, PS2_KEY_F20 },
                { PS2_KC_F21, PS2_KEY_F21 },
                { PS2_KC_F22, PS2_KEY_F22 },
                { PS2_KC_F23, PS2_KEY_F23 },
                { PS2_KC_F24, PS2_KEY_F24 },
                { PS2_KC_KP_COMMA, PS2_KEY_KP_COMMA },
                { PS2_KC_INTL1, PS2_KEY_INTL1 },
                { PS2_KC_INTL2, PS2_KEY_INTL2 },
                { PS2_KC_INTL3, PS2_KEY_INTL3 },
                { PS2_KC_INTL4, PS2_KEY_INTL4 },
                { PS2_KC_INTL5, PS2_KEY_INTL5 },
                { PS2_KC_LANG1, PS2_KEY_LANG1 },
                { PS2_KC_LANG2, PS2_KEY_LANG2 },
                { PS2_KC_LANG3, PS2_KEY_LANG3 },
                { PS2_KC_LANG4, PS2_KEY_LANG4 },
                { PS2_KC_LANG5, PS2_KEY_LANG5 }
                };

/* Two byte Key  table after an E0 byte received */
#if defined(PS2_REQUIRES_PROGMEM)
const uint8_t PROGMEM extended_key[][ 2 ] = {
#else
const uint8_t extended_key[][ 2 ] = {
#endif
                { PS2_KC_IGNORE, PS2_KEY_IGNORE },
                { PS2_KC_PRTSCR, PS2_KEY_PRTSCR },
                { PS2_KC_CTRL, PS2_KEY_R_CTRL },
                { PS2_KC_ALT, PS2_KEY_R_ALT },
                { PS2_KC_L_GUI, PS2_KEY_L_GUI },
                { PS2_KC_R_GUI, PS2_KEY_R_GUI },
                { PS2_KC_MENU, PS2_KEY_MENU },
                { PS2_KC_BREAK, PS2_KEY_BREAK },
                { PS2_KC_HOME, PS2_KEY_HOME },
                { PS2_KC_END, PS2_KEY_END },
                { PS2_KC_PGUP, PS2_KEY_PGUP },
                { PS2_KC_PGDN, PS2_KEY_PGDN },
                { PS2_KC_L_ARROW, PS2_KEY_L_ARROW },
                { PS2_KC_R_ARROW, PS2_KEY_R_ARROW },
                { PS2_KC_UP_ARROW, PS2_KEY_UP_ARROW },
                { PS2_KC_DN_ARROW, PS2_KEY_DN_ARROW },
                { PS2_KC_INSERT, PS2_KEY_INSERT },
                { PS2_KC_DELETE, PS2_KEY_DELETE },
                { PS2_KC_KP_ENTER, PS2_KEY_KP_ENTER },
                { PS2_KC_KP_DIV, PS2_KEY_KP_DIV },
                { PS2_KC_NEXT_TR, PS2_KEY_NEXT_TR },
                { PS2_KC_PREV_TR, PS2_KEY_PREV_TR },
                { PS2_KC_STOP, PS2_KEY_STOP },
                { PS2_KC_PLAY, PS2_KEY_PLAY },
                { PS2_KC_MUTE, PS2_KEY_MUTE },
                { PS2_KC_VOL_UP, PS2_KEY_VOL_UP },
                { PS2_KC_VOL_DN, PS2_KEY_VOL_DN },
                { PS2_KC_MEDIA, PS2_KEY_MEDIA },
                { PS2_KC_EMAIL, PS2_KEY_EMAIL },
                { PS2_KC_CALC, PS2_KEY_CALC },
                { PS2_KC_COMPUTER, PS2_KEY_COMPUTER },
                { PS2_KC_WEB_SEARCH, PS2_KEY_WEB_SEARCH },
                { PS2_KC_WEB_HOME, PS2_KEY_WEB_HOME },
                { PS2_KC_WEB_BACK, PS2_KEY_WEB_BACK },
                { PS2_KC_WEB_FORWARD, PS2_KEY_WEB_FORWARD },
                { PS2_KC_WEB_STOP, PS2_KEY_WEB_STOP },
                { PS2_KC_WEB_REFRESH, PS2_KEY_WEB_REFRESH },
                { PS2_KC_WEB_FAVOR, PS2_KEY_WEB_FAVOR },
                { PS2_KC_POWER, PS2_KEY_POWER },
                { PS2_KC_SLEEP, PS2_KEY_SLEEP },
                { PS2_KC_WAKE, PS2_KEY_WAKE }
                };

/* Scroll lock numeric keypad re-mappings for NOT NUMLOCK */
/* in translated code order order is important */
#if defined(PS2_REQUIRES_PROGMEM)
const uint8_t PROGMEM scroll_remap[] = {
#else
const uint8_t scroll_remap[] = {
#endif
                PS2_KEY_INSERT,     // PS2_KEY_KP0
                PS2_KEY_END,        // PS2_KEY_KP1
                PS2_KEY_DN_ARROW,   // PS2_KEY_KP2
                PS2_KEY_PGDN,       // PS2_KEY_KP3
                PS2_KEY_L_ARROW,    // PS2_KEY_KP4
                PS2_KEY_IGNORE,     // PS2_KEY_KP5
                PS2_KEY_R_ARROW,    // PS2_KEY_KP6
                PS2_KEY_HOME,       // PS2_KEY_KP7
                PS2_KEY_UP_ARROW,   // PS2_KEY_KP8
                PS2_KEY_PGUP,       // PS2_KEY_KP9
                PS2_KEY_DELETE      // PS2_KEY_KP_DOT
                };
#endif
