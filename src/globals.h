#ifndef MAIN_H
#define MAIN_H

#define swGPIOsize 3                                            // Number of key switches
#define swLEDsize 3                                             // Number of key LEDs
#define uLEDsize 2                                              // Number of underglow LEDs
#define swDebounceTime_us 4000                                   //Switch debounce delay in us

#ifdef MAIN_C

extern const uint8_t swGPIO[] = {27, 28, 29};                          // Switch GPIO pins
extern const uint8_t swKeycode[] = {HID_KEY_Z, HID_KEY_X, HID_KEY_C};  // Keybinds

extern const int swLEDGPIO = 26;                                       // Switch LED GPIO pin
extern const int uLEDGPIO = 25;                                        // Underglow LED GPIO pin

#endif
#endif

