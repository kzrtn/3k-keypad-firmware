#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdbool.h>

typedef enum Mode
{
  Mode_Static = 0,
  Mode_RgbCycle,
  Mode_RgbFade,
  Mode_Reactive,
  Mode_ReactiveInverse
} EMode;

bool HandleStateMachine();

#endif