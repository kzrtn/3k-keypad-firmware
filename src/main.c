// 3k keypad for osu!
// Things added:
// - Keypress functionality
// - 1000hz support
// - Reactive switch LEDs
// - Another pio instance for underglow LEDs
// To do:
// - Figure out how to move all lighting to second core
// - Add debounce
// - Add configuration software for light modes and custom key config (somehow...)

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
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#include "usb_descriptors.h"

#define swGPIOsize 3 // Number of key switches
#define swLEDsize 3 // Number of key LEDs
#define uLEDsize 2 // Number of underglow LEDs

const uint8_t swKeycode[] = {HID_KEY_Z, HID_KEY_X, HID_KEY_C};

const uint8_t swGPIO[] = {27, 28, 29};
const int swLEDGPIO = 26;
const int uLEDGPIO = 25;

const PIO pioLeds = pio1;

// Put pixel function
static inline void sw_put_pixel(uint32_t pixel_grb) {
  pio_sm_put_blocking(pioLeds, 0, pixel_grb << 8u);
}

static inline void u_put_pixel(uint32_t pixel_grb) {
  pio_sm_put_blocking(pioLeds, 1, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)(r) << 8) |
        ((uint32_t)(g) << 16) |
        (uint32_t)(b);
}


/*------------- INIT -------------*/
void init() {
  // Set up button pins
  for (int i = 0; i < swGPIOsize; i++) {
    gpio_init(swGPIO[i]);
    gpio_set_function(swGPIO[i], GPIO_FUNC_SIO);
    gpio_set_dir(swGPIO[i], GPIO_IN);
    gpio_pull_up(swGPIO[i]);
  }

  // Set up switch LEDs and underglow LEDs
  uint offsetWs2812 = pio_add_program(pioLeds, &ws2812_program);
  ws2812_program_init(pioLeds, 0, offsetWs2812, swLEDGPIO, 800000, false);
  ws2812_program_init(pioLeds, 1, offsetWs2812, uLEDGPIO, 800000, false);
}

void keyboard() {
  if (tud_hid_ready()) {
    bool isPressed = false;
    uint8_t keycode[6] = {0};
    uint8_t keycodeIndex = 0;

    // Fill keycode array
    // 6 key rollover, a limitation by USB HID, NKRO is possible but... is it even needed?
    for (uint8_t i = 0; i < swGPIOsize; i++) {
      if(!gpio_get(swGPIO[i])) {
        keycode[keycodeIndex] = swKeycode[i];
        keycodeIndex++;
        isPressed = true;
      }
    }
      if (isPressed) {
        // Send key report
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
      }
      else {
        // Send empty report
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
      }

      // Get button input and convert to byte for next step
      uint8_t byte0 = (!gpio_get(swGPIO[0])) ? 0xff : 0x00;
      uint8_t byte1 = (!gpio_get(swGPIO[1])) ? 0xff : 0x00;
      uint8_t byte2 = (!gpio_get(swGPIO[2])) ? 0xff : 0x00;
      uint8_t bytearray[] = {byte0, byte1, byte2};

      // Set array and put pixels
      static uint8_t ledColorData[9] = {};

      for(uint8_t ledNr = 0; ledNr < 3; ledNr++)
      {
        // Set LED data array
        ledColorData[ledNr * 3] = bytearray[ledNr];
        ledColorData[ledNr * 3 + 1] = bytearray[ledNr];
        ledColorData[ledNr * 3 + 2] = bytearray[ledNr];

        // Put pixels
        sw_put_pixel(urgb_u32(
          ledColorData[ledNr * 3],
          ledColorData[ledNr * 3 + 1],
          ledColorData[ledNr * 3 + 2]));
      }
    }
  }


// Core 1 interrupt handler
void core1_interrupt_handler() {

}

/*------------- CORE 1 MAIN -------------*/
void core1_entry() {

}


/*------------- CORE 0 MAIN -------------*/
int main(void)
{
  board_init();
  tusb_init();
  init();

  multicore_launch_core1(core1_entry); // Start core 1 - must be called before configuring interrupts

  // Turn on underglow LEDs
  u_put_pixel(urgb_u32(0xff,0xff,0xff)); //Turn on LED (white)
  u_put_pixel(urgb_u32(0xff,0xff,0xff)); //Turn on LED (white)
  
  while(1) {
    tud_task();
    keyboard();
  }
  return 0;
}

/*------------- USB HID -------------*/
// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;
  (void) report_id;
  (void) buffer;
  (void) buffer;
  (void) bufsize;
}
