#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bsp/board.h"
#include "tusb.h"
#include "pico/stdlib.h"

//multicore stuff
#include "pico/multicore.h"
#include "hardware/irq.h"

//WS2812B stuff
#include "hardware/pio.h"
#include "hardware/clocks.h" // I don't think this is used right now
#include "generated/ws2812.pio.h"

//debounce stuff
#include "hardware/timer.h"
#include "debounce.h"

//#include "LEDs.h"
#include "usb_descriptors.h"

#include "debounce.h"
#include "debounce.c"

#define swGPIOsize 3                                            // Number of key switches
#define swLEDsize 3                                             // Number of key LEDs
#define uLEDsize 2                                              // Number of underglow LEDs
#define swDebounceTime_us 4000                                   //Switch debounce delay in us

#ifdef MAIN_C

const uint8_t swGPIO[] = {27, 28, 29};                          // Switch GPIO pins
const uint8_t swKeycode[] = {HID_KEY_Z, HID_KEY_X, HID_KEY_C};  // Keybinds

const int swLEDGPIO = 26;                                       // Switch LED GPIO pin
const int uLEDGPIO = 25;                                        // Underglow LED GPIO pin

#endif
#endif

