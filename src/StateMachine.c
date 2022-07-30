#include "StateMachine.h"

static EState state = ChooseLed;

bool HandleStateMachine()
{
  // Look for new events to handle
  UpdateEventGenerator();
  
  switch (state)
  {
  case ChooseLed:
    return Handle_ChooseLed();
  case UnderglowConfig:
    return Handle_UnderglowConfig();
  case SwitchLedConfig:
    return Handle_SwitchLedConfig();
  default:
    return false; // Should never reach this part!
  }
}

bool Handle_ChooseLed()
{
  // Do state stuff

  // Check for event
  SEvent event = GetEvent();
  if (event.KeyPressed[0])
  {
    state = UnderglowConfig;
  }
  else if (event.KeyPressed[1])
  {
    state = SwitchLedConfig;
  }

  return true;
}

bool Handle_UnderglowConfig()
{
  // Do state stuff

  // Check for event

  return true;
}

bool Handle_SwitchLedConfig()
{
  // Do state stuff

  // Check for event

  return true;
}