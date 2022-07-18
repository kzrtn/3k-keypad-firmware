// 3k keypad for osu!
// Things added:
// - WS2812B initialisation
// - Basic keypress functionality
// - Empty functions for multicore usage (haven't been used yet though)
// To do:
// - Figure out reactive lighting with WS2812B strips
// - Figure out how to move reactive lightning to second core
// - Add 1000hz support
// - Add debounce
// - Add configuration software (somehow...)

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

#define IS_RGBW true
#define NUM_PIXELS 9
#define WS2812_PIN 26

#include "usb_descriptors.h"

//GPIO pin for switch LEDs is 26
//GPIO pin for underglow LEDs is 25
const uint8_t swKey[] = {HID_KEY_Z, HID_KEY_X, HID_KEY_C};
const uint8_t swGPIO[] = {27, 28, 29};
const size_t swGPIOsize = 3;

/*------------- INIT -------------*/
void init() {
  //set up button pins
  for (int i = 0; i < swGPIOsize; i++) {
    gpio_init(swGPIO[i]);
    gpio_set_function(swGPIO[i], GPIO_FUNC_SIO);
    gpio_set_dir(swGPIO[i], GPIO_IN);
    gpio_pull_up(swGPIO[i]);
  }

  // Set up WS2812
  PIO pio = pio0;
  int sm = 0;
  uint offset = pio_add_program(pio, &ws2812_program);
  ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false);

}

void keyboard() {
  //poll every 1ms? I don't think this actually works
  const uint32_t interval_ms = 1;
  static uint32_t start_ms = 0;

  if(board_millis() - start_ms < interval_ms) return;
  start_ms += interval_ms;

  if (tud_hid_ready()) {
    bool isPressed = false;
  
    for(int i = 0; i < swGPIOsize; i++) {
      if(!gpio_get(swGPIO[i])) {
        //use to avoid send multiple consecutive zero report for keyboard
        uint8_t keycode[6] = {0};
        keycode[0] = swKey[i];

        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);

        isPressed = true;
      }
    }
    // send empty key report if previously has key pressed
      if(!isPressed) {tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);}
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
