#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "EventGenerator.h"
#include <stdbool.h> // TODO: Remove later on

typedef enum State
{
  ChooseLed,
  UnderglowConfig,
  SwitchLedConfig
} EState;

bool HandleStateMachine();

bool Handle_ChooseLed();
bool Handle_UnderglowConfig();
bool Handle_SwitchLedConfig();

#endif