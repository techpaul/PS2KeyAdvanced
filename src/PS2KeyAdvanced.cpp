/* Version V1.0.9
  PS2KeyAdvanced.cpp - PS2KeyAdvanced library
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
                    

  Assumption - Only ONE keyboard added to one Arduino
             - No stream support

  This is for a LATIN style keyboard using Scan code set 2. See various
  websites on what different scan code sets use. Scan Code Set 2 is the
  default scan code set for PS2 keyboards on power up.

  Fully featured PS2 keyboard library to provide
    All keys as a keycode (A-Z and 0-9 as ASCII equivalents)
    All function and movement keys supported even multi-lingual
    Parity checking of data sent/received on receive request keyboard resend
    Resends data when needed handles keyboard protocol for RESEND and ECHO
    Functions for get and set of
        Scancode set in use READ only
        LED and LOCK control
        ReadID
        Reset keyboard
        Send ECHO
        Handles NUM, _CAPS and SCROLL lock keys to LEDs
        Handles NUM/SCROLL internally

  Returns an uint16_t containing
        Make/Break status
        CAPS status
        SHIFT, CTRL, ALT, ALT GR, GUI keys
        Flag for function key not a displayable/printable character
        8 bit key code

    Code Ranges(bottom byte of uint16_t) see PS2KeyAdvanced.h for details
        0       invalid/error
        1-1F    Functions (_CAPS, _SHIFT, _ALT, Enter, DEL... )
        1A-1F   Functions with ASCII control code
                    (DEL, BS, TAB, ESC, ENTER, SPACE)
        20-60   Printable characters noting
                    0-9 = 0x30 to 0x39 as ASCII
                    A to Z = 0x41 to 0x5A as upper case ASCII type codes
                    8B Extra European key
        61-A0   Function keys and other special keys (plus F2 and F1 less 8B)
                    61-78 F1 to F24
                    79-8A Multimedia
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
     handle appropriately value in bottom byte
        0xAA   keyboard has reset and passed power up tests
               will happen if keyboard plugged in after code start
        0xFC   Keyboard General error or power up fail

  It is responsibility of your programme to deal with converting special cases
  like <_CTRL>+<ENTER> sends a special code to something else. A better method
  is to use PS2KeyMap library and add your own table to that library. If you
  wish to do that make a NEW library called SOMETHING different NOT a variant
  or revision of this one, as you are changing base functionality

  See PS2KeyCode.h for codes from the keyboard this library uses to decode.
  (may disappear in updates do not rely on this file or definitions)

  See PS2KeyAvanced.h for returned definitions of Keys and accessible
  definitions

  See PS2KeyMap.h for tables currently supported

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
#include <Arduino.h>
// Internal headers for library defines/codes/etc
#include "PS2KeyAdvanced.h"
#include "PS2KeyCode.h"
#include "PS2KeyTable.h"


// Private function declarations
void send_bit( void );
void send_now( uint8_t );
int16_t send_next( void );
void ps2_reset( void );
uint8_t decode_key( uint8_t );
void pininput( uint8_t );
void set_lock( );

/* Constant control functions to flags array
   in translated key code value order  */
#if defined( PS2_REQUIRES_PROGMEM )
const uint8_t PROGMEM control_flags[] = {
#else
const uint8_t control_flags[] = {
#endif
                _SHIFT, _SHIFT, _CTRL, _CTRL,
                _ALT, _ALT_GR, _GUI, _GUI
                };

// Private Variables
volatile uint8_t _ps2mode;          /* _ps2mode contains
    _PS2_BUSY      bit 7 = busy until all expected bytes RX/TX
    _TX_MODE       bit 6 = direction 1 = TX, 0 = RX (default)
    _BREAK_KEY     bit 5 = break code detected
    _WAIT_RESPONSE bit 4 = expecting data response
    _E0_MODE       bit 3 = in E0 mode
    _E1_MODE       bit 2 = in E1 mode
    _LAST_VALID    bit 1 = last sent valid in case we receive resend
                           and not sent anything */

/* volatile RX buffers and variables accessed via interrupt functions */
volatile uint16_t _rx_buffer[ _RX_BUFFER_SIZE ];     // buffer for data from keyboard
volatile uint8_t _head;              // _head = last byte written
uint8_t _tail;                       // _tail = last byte read (not modified in IRQ ever)
volatile int8_t _bytes_expected;
volatile uint8_t _bitcount;          // Main state variable and bit count for interrupts
volatile uint8_t _shiftdata;
volatile uint8_t _parity;

/* TX variables */
volatile uint8_t _tx_buff[ _TX_BUFFER_SIZE ];    // buffer for keyboard commands
volatile uint8_t _tx_head;          // buffer write pointer
volatile uint8_t _tx_tail;          // buffer read pointer
volatile uint8_t _last_sent;        // last byte if resend requested
volatile uint8_t _now_send;         // immediate byte to send
volatile uint8_t _response_count;   // bytes expected in reply to next TX
volatile uint8_t _tx_ready;         // TX status for type of send contains
            /* _HANDSHAKE 0x80 = handshaking command (ECHO/RESEND)
               _COMMAND   0x01 = other command processing */

/* Output key buffering */
uint16_t _key_buffer[ _KEY_BUFF_SIZE ]; // Output Buffer for translated keys
uint8_t _key_head;                      // Output buffer WR pointer
uint8_t _key_tail;                      // Output buffer RD pointer
uint8_t _mode = 0;            // Mode for output buffer contains
          /* _NO_REPEATS 0x80 No repeat make codes for _CTRL, _ALT, _SHIFT, _GUI
             _NO_BREAKS  0x08 No break codes */

// Arduino settings for pins and interrupts Needed to send data
uint8_t PS2_DataPin;
uint8_t PS2_IrqPin;

// Key decoding variables
uint8_t PS2_led_lock = 0;     // LED and Lock status
uint8_t PS2_lockstate[ 4 ];   // Save if had break on key for locks
uint8_t PS2_keystatus;        // current CAPS etc status for top byte


/*------------------ Code starts here -------------------------*/

/* The ISR for the external interrupt
   To receive 11 bits - start 8 data, ODD parity, stop
   To send data calls send_bit( )
   Interrupt every falling incoming clock edge from keyboard */
void ps2interrupt( void )
{
// Workaround for ESP32 SILICON error see extra/Porting.md
#ifdef PS2_ONLY_CHANGE_IRQ
if( digitalRead( PS2_IrqPin ) )
   return;
#endif
if( _ps2mode & _TX_MODE )
  send_bit( );
else
  {
  static uint32_t prev_ms = 0;
  uint32_t now_ms;
  uint8_t val, ret;

  val = digitalRead( PS2_DataPin );
  /* timeout catch for glitches reset everything */
  now_ms = millis( );
  if( now_ms - prev_ms > 250 )
    {
    _bitcount = 0;
    _shiftdata = 0;
    }
  prev_ms = now_ms;
  _bitcount++;             // Now point to next bit
  switch( _bitcount )
    {
    case 1: // Start bit
            _parity = 0;
            _ps2mode |= _PS2_BUSY;    // set busy
            break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9: // Data bits
            _parity += val;          // another one received ?
            _shiftdata >>= 1;        // right _SHIFT one place for next bit
            _shiftdata |= ( val ) ? 0x80 : 0;    // or in MSbit
            break;
    case 10: // Parity check
            _parity &= 1;            // Get LSB if 1 = odd number of 1's so parity bit should be 0
            if( _parity == val )     // Both same parity error
              _parity = 0xFD;        // To ensure at next bit count clear and discard
            break;
    case 11: // Stop bit lots of spare time now
            if( _parity >= 0xFD )    // had parity error
              {
              send_now( PS2_KC_RESEND );    // request resend
              _tx_ready |= _HANDSHAKE;
              }
            else                    // Good so save byte in _rx_buffer
              {
              // Check _SHIFTed data for commands and action
              ret = decode_key( _shiftdata );
              if( ret & 0x2 )       // decrement expected bytes
                _bytes_expected--;
              if( _bytes_expected <= 0 || ret & 4 )   // Save value ??
                {
                val = _head + 1;
                if( val >= _RX_BUFFER_SIZE )
                  val = 0;
                if( val != _tail )
                  {
                  // get last byte to save
                  _rx_buffer[ val ] = uint16_t( _shiftdata );
                  // save extra details
                  _rx_buffer[ val ] |= uint16_t( _ps2mode ) << 8;
                  _head = val;
                  }
                }
              if( ret & 0x10 )              // Special command to send (ECHO/RESEND)
                {
                send_now( _now_send );
                _tx_ready |= _HANDSHAKE;
                }
              else
                if( _bytes_expected <= 0 )  // Receive data finished
                  {
                  // Set mode and status for next receive byte
                  _ps2mode &= ~( _E0_MODE + _E1_MODE + _WAIT_RESPONSE + _BREAK_KEY );
                  _bytes_expected = 0;
                  _ps2mode &= ~_PS2_BUSY;
                  send_next( );              // Check for more to send
                  }
              }
            _bitcount = 0;	            // end of byte
            break;
    default: // in case of weird error and end of byte reception re-sync
            _bitcount = 0;
    }
  }
}


/* Decode value received to check for errors commands and responses
   NOT keycode translate yet
   returns  bit Or'ing
            0x10 send command in _now_send (after any saves and decrements)
            0x08 error abort reception and reset status and queues
            0x04 save value ( complete after translation )
            0x02 decrement count of bytes to expected

   Codes like EE, AA and FC ( Echo, BAT pass and fail) treated as valid codes 
   return code 6
*/
uint8_t decode_key( uint8_t value )
{
uint8_t state;

state = 6;             // default state save and decrement

// Anything but resend received clear valid value to resend
if( value != PS2_KC_RESEND )
  _ps2mode &= ~( _LAST_VALID );

// First check not a valid response code from a host command
if( _ps2mode & _WAIT_RESPONSE )
  if( value < 0xF0 )
    return state;      // Save response and decrement

// E1 Pause mode  special case just decrement
if( _ps2mode & _E1_MODE )
  return 2;

switch( value )
   {
   case 0:      // Buffer overrun Errors Reset modes and buffers
   case PS2_KC_OVERRUN:
                ps2_reset( );
                state = 0xC;
                break;
   case PS2_KC_RESEND:   // Resend last byte if we have sent something
                if( ( _ps2mode & _LAST_VALID ) )
                  {
                  _now_send = _last_sent;
                  state = 0x10;
                  }
                else
                  state = 0;
                break;
   case PS2_KC_ERROR: // General error pass up but stop any sending or receiving
                _bytes_expected = 0;
                _ps2mode = 0;
                _tx_ready = 0;
                state = 0xE;
                break;
   case PS2_KC_KEYBREAK:   // break Code - wait the final key byte
                _bytes_expected = 1;
                _ps2mode |= _BREAK_KEY;
                state = 0;
                break;
   case PS2_KC_ECHO:   // Echo if we did not originate echo back
                state = 4;                  // always save
                if( _ps2mode & _LAST_VALID && _last_sent != PS2_KC_ECHO )
                  {
                  _now_send = PS2_KC_ECHO;
                  state |= 0x10;            // send _command on exit
                  }
                break;
   case PS2_KC_BAT:     // BAT pass
                _bytes_expected = 0;         // reset as if in middle of something lost now
                state = 4;
                break;
   case PS2_KC_EXTEND1:   // Major extend code (PAUSE key only)
                if( !( _ps2mode & _E1_MODE ) )  // First E1 only
                  {
                  _bytes_expected = 7;       // seven more bytes
                  _ps2mode |= _E1_MODE;
                  _ps2mode &= ~_BREAK_KEY;    // Always a make
                  }
                state = 0;
                break;
   case PS2_KC_EXTEND:   // Two byte Extend code
                _bytes_expected = 1;        // one more byte at least to wait for
                _ps2mode |= _E0_MODE;
                state = 0;
                break;
   }
return state;
}


/* Send data to keyboard
   Data pin direction should already be changed
   Start bit would be already set so each clock setup for next clock
   parity and _bitcount should be 0 already and busy should be set

   Start bit setting is due to bug in attachinterrupt not clearing pending interrupts
   Also no clear pending interrupt function   */
void send_bit( void )
{
uint8_t val;

_bitcount++;               // Now point to next bit
switch( _bitcount )
  {
  case 1: 
#if defined( PS2_CLEAR_PENDING_IRQ ) 
          // Start bit due to Arduino bug
          digitalWrite( PS2_DataPin, LOW );
          break;
#endif
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
          // Data bits
          val = _shiftdata & 0x01;   // get LSB
          digitalWrite( PS2_DataPin, val ); // send start bit
          _parity += val;            // another one received ?
          _shiftdata >>= 1;          // right _SHIFT one place for next bit
          break;
  case 10:
          // Parity - Send LSB if 1 = odd number of 1's so parity should be 0
          digitalWrite( PS2_DataPin, ( ~_parity & 1 ) );
          break;
  case 11: // Stop bit write change to input pull up for high stop bit
          pininput( PS2_DataPin );
          break;
  case 12: // Acknowledge bit low we cannot do anything if high instead of low
          if( !( _now_send == PS2_KC_ECHO || _now_send == PS2_KC_RESEND ) )
            {
            _last_sent = _now_send;   // save in case of resend request
            _ps2mode |= _LAST_VALID;
            }
          // clear modes to receive again
          _ps2mode &= ~_TX_MODE;
          if( _tx_ready & _HANDSHAKE )      // If _HANDSHAKE done
            _tx_ready &= ~_HANDSHAKE;
          else                              // else we finished a command
            _tx_ready &= ~_COMMAND;
          if( !( _ps2mode & _WAIT_RESPONSE ) )   //  if not wait response
            send_next( );                    // check anything else to queue up
          _bitcount = 0;	            // end of byte
          break;
  default: // in case of weird error and end of byte reception re-sync
          _bitcount = 0;
  }
}


/* Takes a byte sets up variables and starts the data sending processes
   Starts the actual byte transmission
   calling code must make sure line is idle and able to send
   Whilst this function adds long delays the process of the delays
   will STOP the interrupt source (keyboard) externally when clock held low
   _tx_ready contains 2 flags checked in this order
    _HANDSHAKE   command sent as part of receiving e.g. ECHO, RESEND
    _COMMAND     other commands not part of receiving
  Main difference _bytes_expected is NOT altered in _HANDSHAKE mode
  in command mode we update _bytes_expected with number of response bytes
*/
void send_now( uint8_t command )
{
_shiftdata = command;
_now_send = command;     // copy for later to save in last sent
#if defined( PS2_CLEAR_PENDING_IRQ ) 
_bitcount = 0;          // AVR/SAM ignore extra interrupt
#else
_bitcount = 1;          // Normal processors
#endif
_parity = 0;
_ps2mode |= _TX_MODE + _PS2_BUSY;

// Only do this if sending a command not from Handshaking
if( !( _tx_ready & _HANDSHAKE ) && ( _tx_ready & _COMMAND ) )
  {
  _bytes_expected = _response_count;  // How many bytes command will generate
  _ps2mode |= _WAIT_RESPONSE;
  }

// STOP interrupt handler 
// Setting pin output low will cause interrupt before ready
detachInterrupt( digitalPinToInterrupt( PS2_IrqPin ) );
// set pins to outputs and high
digitalWrite( PS2_DataPin, HIGH );
pinMode( PS2_DataPin, OUTPUT );
digitalWrite( PS2_IrqPin, HIGH );
pinMode( PS2_IrqPin, OUTPUT );
// Essential for PS2 spec compliance
delayMicroseconds( 10 );
// set Clock LOW
digitalWrite( PS2_IrqPin, LOW );
// Essential for PS2 spec compliance
// set clock low for 60us
delayMicroseconds( 60 );
// Set data low - Start bit
digitalWrite( PS2_DataPin, LOW );
// set clock to input_pullup data stays output while writing to keyboard
pininput( PS2_IrqPin );
// Restart interrupt handler
attachInterrupt( digitalPinToInterrupt( PS2_IrqPin ), ps2interrupt, FALLING );
//  wait clock interrupt to send data
}


/* Send next byte/command from TX queue and start sending
   Must be ready to send and idle
   Assumes commands consist of 1 or more bytes and wait for response then may or
   not be followed by further bytes to send with or without response
    Checks
    1/ Buffer empty return empty buffer
    2/ Busy return busy (will be checked by interrupt routines later)
    3/ Read next byte (next byte to send)
    4/ Check if following byte(s) are command/data or response

    Returns  1 if started transmission or queued
            -134 if already busy
            -2 if buffer empty

    Note PS2_KEY_IGNORE is used to denote a byte(s) expected in response */
int16_t send_next( void )
{
uint8_t  i;
int16_t  val;

val = -1;
// Check buffer not empty
i = _tx_tail;
if( i == _tx_head )
  return -2;

// set command bit in _tx_ready as another command to do
_tx_ready |= _COMMAND;

// Already item waiting to be sent or sending interrupt routines will call back
if( _tx_ready & _HANDSHAKE )
  return -134;

// if busy let interrupt catch and call us again
if( _ps2mode & _PS2_BUSY )
  return -134;

// Following only accessed when not receiving or sending protocol bytes
// Scan for command response and expected bytes to follow
_response_count = 0;
do
  {
  i++;
  if( i >= _TX_BUFFER_SIZE )
    i = 0;
  if( val == -1 )
    val = _tx_buff[ i ];
  else
    if( _tx_buff[ i ] != PS2_KEY_IGNORE )
      break;
    else
      _response_count++;
  _tx_tail = i;
  }
while( i != _tx_head );
// Now know what to send and expect start the actual wire sending
send_now( val );
return 1;
}


/*  Send a byte to the TX buffer
    Value in buffer of PS2_KEY_IGNORE signifies wait for response,
    use one for each byte expected

    Returns -4 - if buffer full (buffer overrun not written)
    Returns 1 byte written when done */
int send_byte( uint8_t val )
{
uint8_t ret;

ret = _tx_head + 1;
if( ret >= _TX_BUFFER_SIZE )
  ret = 0;
if( ret != _tx_tail )
  {
  _tx_buff[ ret ] = val;
  _tx_head = ret;
  return 1;
  }
return -4;
}


// initialize a data pin for input
void pininput( uint8_t pin )
{
#ifdef INPUT_PULLUP
pinMode( pin, INPUT_PULLUP );
#else
digitalWrite( pin, HIGH );
pinMode( pin, INPUT );
#endif
}


void ps2_reset( void )
{
/* reset buffers and states */
_tx_head = 0;
_tx_tail = 0;
_tx_ready = 0;
_response_count = 0;
_head = 0;
_tail = 0;
_bitcount = 0;
PS2_keystatus = 0;
PS2_led_lock = 0;
_ps2mode = 0;
}


uint8_t key_available( )
{
int8_t  i;

i = _head - _tail;
if( i < 0 )
  i += _RX_BUFFER_SIZE;
return uint8_t( i );
}


/*  Translate PS2 keyboard code sequence into our key code data
    PAUSE key (_E1_MODE) is de_ALT with as special case, and
    command responses not translated

    Called from read function as too long for in interrupt

    Returns 0 for no valid key or processed internally ignored or similar
            0 for empty buffer
    */
uint16_t translate( void )
{
uint8_t   index, length, data;
uint16_t  retdata;

// get next character
// Check first something to fetch
index = _tail;
// check for empty buffer
if( index == _head )
  return 0;
index++;
if( index >= _RX_BUFFER_SIZE )
  index = 0;
_tail = index;
// Get the flags byte break modes etc in this order
data = _rx_buffer[ index ] & 0xFF;
index = ( _rx_buffer[ index ] & 0xFF00 ) >> 8;

// Catch special case of PAUSE key
if( index & _E1_MODE )
  return  PS2_KEY_PAUSE + _FUNCTION;

// Ignore anything not actual keycode but command/response
// Return untranslated as valid
if( ( data >= PS2_KC_BAT && data != PS2_KC_LANG1 && data != PS2_KC_LANG2 )
    || ( index & _WAIT_RESPONSE ) )
  return ( uint16_t )data;

// Gather the break of key status
if( index & _BREAK_KEY )
  PS2_keystatus |= _BREAK;
else
  PS2_keystatus &= ~_BREAK;

retdata = 0;    // error code by default
// Scan appropriate table
if( index & _E0_MODE )
  {
  length = sizeof( extended_key ) / sizeof( extended_key[ 0 ] );
  for( index = 0; index < length; index++ )
#if defined( PS2_REQUIRES_PROGMEM )
     if( data == pgm_read_byte( &extended_key[ index ][ 0 ] ) )
       {
       retdata = pgm_read_byte( &extended_key[ index ][ 1 ] );
#else
     if( data == extended_key[ index ][ 0 ] )
       {
       retdata = extended_key[ index ][ 1 ];
#endif
       break;
       }
  }
else
  {
  length = sizeof( single_key ) / sizeof( single_key[ 0 ] );
  for( index = 0; index < length; index++ )
#if defined( PS2_REQUIRES_PROGMEM )
     if( data == pgm_read_byte( &single_key[ index ][ 0 ] ) )
       {
       retdata = pgm_read_byte( &single_key[ index ][ 1 ] );
#else
     if( data == single_key[ index ][ 0 ] )
       {
       retdata = single_key[ index ][ 1 ];
#endif
       break;
       }
  }
// trap not found key
if( index == length )
  retdata = 0;
/* valid found values only */
if( retdata > 0 )
  {
  if( retdata <= PS2_KEY_CAPS )
    {   // process lock keys need second make to turn off
    if( PS2_keystatus & _BREAK )
      {
      PS2_lockstate[ retdata ] = 0; // Set received a break so next make toggles LOCK status
      retdata = PS2_KEY_IGNORE;     // ignore key
      }
    else
      {
      if( PS2_lockstate[ retdata ] == 1 )
        retdata = PS2_KEY_IGNORE;   // ignore key if make and not received break
      else
        {
        PS2_lockstate[ retdata ] = 1;
        switch( retdata )
          {
          case PS2_KEY_CAPS:   index = PS2_LOCK_CAPS;
                               // Set CAPS lock if not set before
                               if( PS2_keystatus & _CAPS )
                                 PS2_keystatus &= ~_CAPS;
                               else
                                 PS2_keystatus |= _CAPS;
                               break;
          case PS2_KEY_SCROLL: index = PS2_LOCK_SCROLL;
                               break;
          case PS2_KEY_NUM:    index = PS2_LOCK_NUM;
                               break;
          }
        // Now update PS2_led_lock status to match
        if( PS2_led_lock & index )
          {
          PS2_led_lock &= ~index;
          PS2_keystatus |= _BREAK;     // send as break
          }
        else
          PS2_led_lock |= index;
        set_lock( );
        }
      }
    }
  else
    if( retdata >= PS2_KEY_L_SHIFT && retdata <= PS2_KEY_R_GUI )
      { // Update bits for _SHIFT, _CTRL, _ALT, _ALT GR, _GUI in status
#if defined( PS2_REQUIRES_PROGMEM )
      index = pgm_read_byte( &control_flags[ retdata - PS2_KEY_L_SHIFT ] );
#else
      index = control_flags[ retdata - PS2_KEY_L_SHIFT ];
#endif
      if( PS2_keystatus & _BREAK )
        PS2_keystatus &= ~index;
      else
        // if already set ignore repeats if flag set
        if( ( PS2_keystatus & index ) && ( _mode & _NO_REPEATS ) )
          retdata = PS2_KEY_IGNORE; // ignore repeat _SHIFT, _CTRL, _ALT, _GUI
        else
          PS2_keystatus |= index;
      }
    else
      // Numeric keypad ONLY works in numlock state or when _SHIFT status
      if( retdata >= PS2_KEY_KP0 && retdata <=  PS2_KEY_KP_DOT )
        if( !( PS2_led_lock & PS2_LOCK_NUM ) || ( PS2_keystatus & _SHIFT ) )
#if defined( PS2_REQUIRES_PROGMEM )
          retdata = pgm_read_byte( &scroll_remap[ retdata - PS2_KEY_KP0 ] );
#else
          retdata = scroll_remap[ retdata - PS2_KEY_KP0 ];
#endif
  // Sort break code handling or ignore for all having processed the _SHIFT etc status
  if( ( PS2_keystatus & _BREAK ) && ( _mode & _NO_BREAKS ) )
    return ( uint16_t )PS2_KEY_IGNORE;
  // Assign Function keys _mode
  if( ( retdata <= PS2_KEY_SPACE || retdata >= PS2_KEY_F1 ) && retdata != PS2_KEY_EUROPE2 )
    PS2_keystatus |= _FUNCTION;
  else
    PS2_keystatus &= ~_FUNCTION;
  }
return ( retdata | ( (uint16_t)PS2_keystatus << 8 ) );
}


/* Build command to send lock status
    Assumes data is within range */
void set_lock( )
{
send_byte( PS2_KC_LOCK );        // send command
send_byte( PS2_KEY_IGNORE );     // wait ACK
send_byte( PS2_led_lock );       // send data from internal variable
if( ( send_byte( PS2_KEY_IGNORE ) ) ) // wait ACK
  send_next( );              // if idle start transmission
}


/*  Send echo command to keyboard
    returned data in keyboard buffer read as keys */
void PS2KeyAdvanced::echo( void )
{
send_byte( PS2_KC_ECHO );             // send command
if( ( send_byte( PS2_KEY_IGNORE ) ) ) // wait data PS2_KC_ECHO
  send_next( );                   // if idle start transmission
}


/*  Get the ID used in keyboard
    returned data in keyboard buffer read as keys */
void PS2KeyAdvanced::readID( void )
{
send_byte( PS2_KC_READID );           // send command
send_byte( PS2_KEY_IGNORE );          // wait ACK
send_byte( PS2_KEY_IGNORE );          // wait data
if( ( send_byte( PS2_KEY_IGNORE ) ) ) // wait data
  send_next( );                   // if idle start transmission
}


/*  Get the current Scancode Set used in keyboard
    returned data in keyboard buffer read as keys */
void PS2KeyAdvanced::getScanCodeSet( void )
{
send_byte( PS2_KC_SCANCODE );         // send command
send_byte( PS2_KEY_IGNORE );          // wait ACK
send_byte( 0 );                       // send data 0 = read
send_byte( PS2_KEY_IGNORE );          // wait ACK
if( ( send_byte( PS2_KEY_IGNORE ) ) ) // wait data
  send_next( );                   // if idle start transmission
}


/* Returns the current status of Locks */
uint8_t PS2KeyAdvanced::getLock( )
{
return( PS2_led_lock );
}


/* Sets the current status of Locks and LEDs */
void PS2KeyAdvanced::setLock( uint8_t code )
{
code &= 0xF;                // To allow for rare keyboards with extra LED
PS2_led_lock = code;        // update our lock copy
PS2_keystatus &= ~_CAPS;    // Update copy of _CAPS lock as well
PS2_keystatus |= ( code & PS2_LOCK_CAPS ) ? _CAPS : 0;
set_lock( );
}


/* Set library to not send break key codes
            1 = no break codes
            0 = send break codes  */
void PS2KeyAdvanced::setNoBreak( uint8_t data )
{
_mode &= ~_NO_BREAKS;
_mode |= data ? _NO_BREAKS : 0;
}

 /* Set library to not repeat make codes for _CTRL, _ALT, _GUI, _SHIFT
            1 = no repeat codes
            0 = send repeat codes  */
void PS2KeyAdvanced::setNoRepeat( uint8_t data )
{
_mode &= ~_NO_REPEATS;
_mode |= data ? _NO_REPEATS : 0;
}


/* Resets keyboard when reset has completed
   keyboard sends AA - Pass or FC for fail        */
void PS2KeyAdvanced::resetKey( )
{
send_byte( PS2_KC_RESET );            // send command
send_byte( PS2_KEY_IGNORE );          // wait ACK
if( ( send_byte( PS2_KEY_IGNORE ) ) ) // wait data PS2_KC_BAT or PS2_KC_ERROR
  send_next( );                        // if idle start transmission
// LEDs and KeyStatus Reset too... to match keyboard
PS2_led_lock = 0;
PS2_keystatus = 0;
}


/*  Send Typematic rate/delay command to keyboard
    First Parameter  rate is 0 - 0x1F (31)
                0 = 30 CPS
                0x1F = 2 CPS
                default in keyboard is 0xB (10.9 CPS)
    Second Parameter delay is 0 - 3 for 0.25s to 1s in 0.25 increments
        default in keyboard is 1 = 0.5 second delay
    Returned data in keyboard buffer read as keys

    Error returns 0 OK
                -5 parameter error
                */
int PS2KeyAdvanced::typematic( uint8_t rate, uint8_t delay )
{
if( rate > 31 || delay > 3 )
  return -5;
send_byte( PS2_KC_RATE );             // send command
send_byte( PS2_KEY_IGNORE );          // wait ACK
send_byte( ( delay << 5 ) + rate );   // Send values
if( ( send_byte( PS2_KEY_IGNORE ) ) ) // wait ACK
  send_next( );                   // if idle start transmission
return 0;
}


/* Returns count of available processed key codes

   If processed key buffer (_key_buffer) buffer returns max count
   else processes input key code buffer until
     either input buffer empty
         or output buffer full
     returns actual count

  Returns   0 buffer empty
            1 to buffer size less 1 as 1 to full buffer

  As with other ring buffers here when pointers match
  buffer empty so cannot actually hold buffer size values  */
uint8_t PS2KeyAdvanced::available( )
{
int8_t  i, idx;
uint16_t data;

// check output queue
i = _key_head - _key_tail;
if( i < 0 )
  i += _KEY_BUFF_SIZE;
while( i < ( _KEY_BUFF_SIZE - 1 ) ) // process if not full
  if( key_available( ) )         // not check for more keys to process
    {
    data = translate( );         // get next translated key
    if( data == 0 )             // unless in buffer is empty
      break;
    if( ( data & 0xFF ) != PS2_KEY_IGNORE
            && ( data & 0xFF ) > 0 )
      {
      idx = _key_head + 1;         // point to next space
      if( idx >= _KEY_BUFF_SIZE )  // loop to front if necessary
        idx = 0;
      _key_buffer[ idx ] = data; // save the data to out buffer
      _key_head = idx;
      i++;                      // update count
      }
    }
  else
    break;                      // exit nothing coming in
return uint8_t( i );
}


/* read a decoded key from the keyboard buffer
   returns 0 for empty buffer */
uint16_t PS2KeyAdvanced::read( )
{
uint16_t result;
uint8_t idx;

if( ( result = available( ) ) )
  {
  idx = _key_tail;
  idx++;
  if( idx >= _KEY_BUFF_SIZE )  // loop to front if necessary
    idx = 0;
  _key_tail = idx;
  result = _key_buffer[ idx ];
  }
return result;
}


PS2KeyAdvanced::PS2KeyAdvanced( )
{
// nothing to do here, begin( ) does it all
}


/* instantiate class for keyboard  */
void PS2KeyAdvanced::begin( uint8_t data_pin, uint8_t irq_pin )
{
/* PS2 variables reset */
ps2_reset( );

PS2_DataPin = data_pin;
PS2_IrqPin = irq_pin;

// initialize the pins
pininput( PS2_IrqPin );            /* Setup Clock pin */
pininput( PS2_DataPin );           /* Setup Data pin */

// Start interrupt handler
attachInterrupt( digitalPinToInterrupt( irq_pin ), ps2interrupt, FALLING );
}
