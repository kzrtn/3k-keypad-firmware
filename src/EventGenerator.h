#ifndef EVENT_GENERATOR_H
#define EVENT_GENERATOR_H

#include <stdbool.h>

typedef struct Event
{
  bool KeyPressed[3];
} SEvent;

void UpdateEventGenerator();
SEvent GetEvent();

#endif