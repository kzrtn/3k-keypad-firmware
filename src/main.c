// 3k keypad for osu!
// Things added:
// - WS2812B initialisation
// - Basic keypress functionality
// - Empty functions for multicore usage (haven't been used yet though)
// - All switch LEDs light up when any key is pressed
// - Another pio instance for underglow LEDs
// To do:
// - Figure out how to get LEDs to light up individually
// - Figure out how to get underglow LEDs to even work (I AM IN PAIN)
// - Figure out how to move all lightning to second core
// - Add 1000hz support
// - Add debounce
// - Add lighting colour modes
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
#include "generated/ws2812.pio.h"

#include "usb_descriptors.h"

const uint8_t swKey[] = {HID_KEY_Z, HID_KEY_X, HID_KEY_C};
const uint8_t swGPIO[] = {27, 28, 29};
const size_t swGPIOsize = 3;
const int swLEDGPIO = 26;
const int uLEDGPIO = 25;

//put pixel function
static inline void put_pixel(uint32_t pixel_grb) {
  pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

//would this even work? does not seem to be working
static inline void put_pixel_1(uint32_t pixel_grb) {
  pio_sm_put_blocking(pio1, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)(r) << 8) |
        ((uint32_t)(g) << 16) |
        (uint32_t)(b);
}


/*------------- INIT -------------*/
void init() {
  //set up button pins
  for (int i = 0; i < swGPIOsize; i++) {
    gpio_init(swGPIO[i]);
    gpio_set_function(swGPIO[i], GPIO_FUNC_SIO);
    gpio_set_dir(swGPIO[i], GPIO_IN);
    gpio_pull_up(swGPIO[i]);
  }
  // Set up switch LEDs
  PIO pio = pio0;
  int sm = 0;
  uint offset = pio_add_program(pio, &ws2812_program);
  ws2812_program_init(pio, sm, offset, swLEDGPIO, 800000, false);

  // Set up underglow LEDs
  PIO pio_1 = pio1;
  int sm1 = 1;
  uint offset1 = pio_add_program(pio_1, &ws2812_program);
  ws2812_program_init(pio_1, sm1, offset1, uLEDGPIO, 800000, false);
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
        
        //switch LED lights up when keys are pressed
        put_pixel(urgb_u32(0xff,0xff,0xff)); //Turn on LED (white)
        put_pixel(urgb_u32(0xff,0xff,0xff)); //Turn on LED (white)
        put_pixel(urgb_u32(0xff,0xff,0xff)); //Turn on LED (white)
        isPressed = true;
      }
    }
    // send empty key report if previously has key pressed
    if(!isPressed) {tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);}

    // clear LEDs
    for(int i=0; i<3; i++){
      put_pixel(urgb_u32(0,0,0)); //Turn off LEDs
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

  //turn on underglow LEDs (it's not working though)
  put_pixel_1(urgb_u32(0xff,0,0)); //Turn on LED (red)
  put_pixel_1(urgb_u32(0,0xff,0)); //Turn on LED (green)
  
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
