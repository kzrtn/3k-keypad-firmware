#include "Animations.h"

// Make sure the contents of the functions are non-blocking.
// So no delay or while(true) stuff

// Also make sure the animation speed is independant of how often the function is called
// E.g. if the RGB animation takes 1 second to go from red to blue then the color should be blue after 1 second.
// Wether the function was called 1 times or 10000 should not influence which color is output after 1 second.

SRgb GetColorRgbCycle()
{
  // TODO: Implement
  SRgb temporary;
  temporary.Red = 0;
  temporary.Green = 0;
  temporary.Blue = 0;
  return temporary;
}

SRgb GetColorRgbFade()
{
  // TODO: Implement
  SRgb temporary;
  temporary.Red = 0;
  temporary.Green = 0;
  temporary.Blue = 0;
  return temporary;
}