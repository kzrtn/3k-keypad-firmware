#ifndef COLOR_CONVERSION_H
#define COLOR_CONVERSION_H

#include "pico/stdlib.h"

typedef struct Hsv
{
  float Hue;        // 0 to 360
  float Saturation; // 0 to 1
  float Value;      // 0 to 1
} SHsv;

typedef union Rgb
{
  struct
  {
    uint8_t Blue;
    uint8_t Red;
    uint8_t Green;
  };

  uint32_t Data;
} SRgb;

SRgb GetRgbFromHsv(SHsv hsv);
SHsv GetHsvFromRgb(SRgb rgb);

#endif