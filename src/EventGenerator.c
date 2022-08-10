#include "EventGenerator.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include "debounce.pio.h"

extern const uint8_t swGPIOsize;
extern const PIO pioDebounce;

static SEvent event = {0};
static bool buttonStatePrev[] = {true, true, true}; // State machine is entered with button states true
static uint64_t pressTimestampUs[] = {0, 0, 0};
static uint32_t eventRepeatCount[] = {0, 0, 0};

static uint32_t holdUntilScrollingUs = 1000000;
static uint32_t repeatEventDelayUs = 50000;

void UpdateEventGenerator()
{
  bool buttonStateCurr[3];

  for (uint8_t i = 0; i < swGPIOsize; i++)
    buttonStateCurr[i] = debounce_program_get_button_pressed(pioDebounce, i);

  for (uint8_t i = 0; i < swGPIOsize; i++)
  {
    event.KeyPressed[i] = false; // Clear event in case it was true in the previous update

    if (buttonStateCurr[i] != buttonStatePrev[i]) // Button state has changed
    {
      if (buttonStateCurr[i] == true) // Button was pressed
      {
        event.KeyPressed[i] = true;
        pressTimestampUs[i] = time_us_64();
        eventRepeatCount[i] = 0;
      }
      
      buttonStatePrev[i] = buttonStateCurr[i];
    }
    else if (buttonStateCurr[i]) // Button is pressed
    {
      if (time_us_64() > pressTimestampUs[i] + holdUntilScrollingUs + (repeatEventDelayUs * eventRepeatCount[i]))
      {
        event.KeyPressed[i] = true;
        eventRepeatCount[i]++;
      }
    }  
  }
}

SEvent GetEvent()
{
  return event;
}
