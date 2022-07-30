#include "EventGenerator.h"

static SEvent event = {};
static bool buttonStatePrev[] = {false, false, false};

void UpdateEventGenerator()
{
  bool buttonStateCurr[3];
  buttonStateCurr[0] = false; // TODO: Get the button states and put them in the array
  buttonStateCurr[1] = false;
  buttonStateCurr[2] = false;

  for (int i = 0; i < 3; i++)
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
