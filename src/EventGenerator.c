#include "EventGenerator.h"
#include "hardware/clocks.h"
#include "debounce.pio.h"

extern const uint8_t swGPIOsize;
extern const PIO pioDebounce;

static SEvent event = {0};
static bool buttonStatePrev[] = {true, true, true}; // State machine is entered with button states true

void UpdateEventGenerator()
{
  bool buttonStateCurr[3];

  for (uint8_t i = 0; i < swGPIOsize; i++)
    buttonStateCurr[0] = debounce_program_get_button_pressed(pioDebounce, i);

  for (uint8_t i = 0; i < swGPIOsize; i++)
  {
    event.KeyPressed[i] = false; // Clear event in case it was true in the previous update

    if (buttonStateCurr[i] != buttonStatePrev[i]) // Button state has changed
    {
      if (buttonStateCurr[i] == true) // Button was pressed
      {
        event.KeyPressed[i] = true;
      }
      
      buttonStatePrev[i] = buttonStateCurr[i];
    }
  }
}

SEvent GetEvent()
{
  return event;
}
