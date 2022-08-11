#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "ColorConversion.h"

SRgb GetColorRgbCycle(uint64_t* prevTime, float speed, SHsv* hsv);
SRgb GetColorRgbFade(); 

#endif