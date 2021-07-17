# PS2KeyAdvanced - Porting to New Boards or Architectures
This document assumes you have some low level understanding of your board, Arduino library structures and the compiler for your board.

The majority of assists you require are in PS2KeyAdvanced.h the main header file for the library which has some architecture specific defines.

When a valid supported architecture is supported this flag is set. Without this set for a **VALID** architecture that is supported
a compiler warning or hard error is reported on compiling.

-  PS2_SUPPORTED

These are to cope with AVR Harvard architecture, AVR and SAM (many but not all architectures) issue of *not* clearing past events on Interrupt pin 
before attachInterrupt enables interrupts those causing false interrupt events.

-  PS2_REQUIRES_PROGMEM
-  PS2_CLEAR_PENDING_IRQ

The following is to cope with architectures that use CHANGE interrupt even when asking for FALLING.
-  PS2_ONLY_CHANGE_IRQ

## General Rules
To add another board type you need to make some changes
1.  You need to determine what the define is for your architecture, as AVR uses ARDUINO_ARCH_AVR so from verbose compiling output you need to find 
the part in compiling output for YOUR BOARD that should be **-DARDUINO_ARCH_xxxx**. Where the "xxxx" is the board architecture. You need this for 
making later changes.
2.  In PS2KeyAdvanced.h you need to add a specific test for your board after the AVR and SAM checks (Note PS2_SUPPORTED **must** be included in your 
test otherwise a compilation error will occur.) like
~~~
#if defined( ARDUINO_ARCH_xxxx )
#define PS2_SUPPORTED           1
#define PS2_REQUIRES_PROGMEM    1
#endif
~~~
3.  Change library.properties to add your architecture to the comma separated list for your architecture, normally this is the "xxxx" for ARCH_ARDUINO_xxxx

If your architecture needs an **EXTRA** flag then it must have a name describing the functionality the flag enables
**NOT** the board or architecture name. this means it is easier to be readable to others and may also be needed for other
other boards or architectures.
### PS2_SUPPORTED
This flag is set to indicate we have a supported board, this flag stops a compiler error being forced.

To enable for your architecture add a line in the '#if' for your architecture as follows
~~~
#define PS2_SUPPORTED       1
~~~
### PS2_REQUIRES_PROGMEM
This determines that to have constants in Flash memory not RAM on AVR boards the PROGMEM functions have to be used.

To enable for your architecture add a line in the '#if' for your architecture as follows
~~~
#define PS2_REQUIRES_PROGMEM    1
~~~
### PS2_CLEAR_PENDING_IRQ
When sending data to the keyboard, interrupts have to be turned off and back on again, on AVR and SAM architecture any changes on the clock pin 
cause extra interrupts when attachInterrupt is used to start bit timing interrupts again, on other architectures this does not normally happen so we 
need to change how many interrupt events we see for a valid byte sending. This does not affect receiving data just the sending.

This defines adds an extra bit clock event interrupt step for AVR and SAM

To enable for your architecture add a line in the '#if' for your architecture as follows
~~~
#define PS2_CLEAR_PENDING_IRQ    1
~~~
### PS2_ONLY_CHANGE_IRQ
This came about when the processor or the board package does not support all types of interrupt, the library uses 
FALLING (falling edge when signal goes from high to low), some processors or board packages do not implement this 
and use CHANGE interrupt (both edges) even if you ask for one edge.

To enable for your architecture add a line in the '#if' for your architecture as follows
~~~
#define PS2_ONLY_CHANGE_IRQ    1
~~~
This was first required for ESP32, which has a SILICON (hardware) MCU problem and workarounds **SHOULD** have been incorporated into arduino-esp32 
https://github.com/espressif/arduino-esp32 The issue there are **SPURIOUS** extra interrupts.
## Support of changes
If you can test **ALL** functionality working, you are welcome to do a pull request from your github fork of this library. If this meets our coding 
guidelines and you can show it is has been working it will include in next release.

Please note whilst we can give assistance, we probably do not have your board or maybe not the time to incorporate full support for you. of course we
can always accept standard commercial daily rates to this for your, if you are made of money and willing for result to be publicly available for free.
