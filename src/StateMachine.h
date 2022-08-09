#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdbool.h>

typedef enum Mode
{
  Mode_Reactive = 0,
  Mode_Static,
  Mode_RgbCycle,
  Mode_RgbFade
} EMode;

bool HandleStateMachine();

#endif