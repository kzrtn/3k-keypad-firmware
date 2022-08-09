#include "ColorConversion.h"
#include "math.h"

inline static uint8_t Min(uint8_t val1, uint8_t val2)
{
  return (val1 < val2) ? val1 : val2;
}

inline static uint8_t Max(uint8_t val1, uint8_t val2)
{
  return (val1 > val2) ? val1 : val2;
}

static inline float radiansToDegrees(float radians)
{ 
  return (radians * 57.2957795f);
}

// Conversion formulas from: https://www.had2know.org/technology/hsv-rgb-conversion-formula-calculator.html

SRgb GetRgbFromHsv(SHsv hsv)
{
  SRgb rgb = {0};

  float M = hsv.Value * 255.0f;
  float m = M * (1.0f - hsv.Saturation);
  float z = (M - m) * (1 - fabsf( fmod(hsv.Hue / 60.0f, 2) - 1) );

  if (hsv.Hue < 60.0f)
  {
    rgb.Red = (uint8_t)M;
    rgb.Green = (uint8_t)(z + m);
    rgb.Blue = (uint8_t)m;
  }
  else if (hsv.Hue < 120.0f)
  {
    rgb.Red = (uint8_t)(z + m);
    rgb.Green = (uint8_t)M;
    rgb.Blue = (uint8_t)m;
  }
  else if (hsv.Hue < 180.0f)
  {
    rgb.Red = (uint8_t)m;
    rgb.Green = (uint8_t)M;
    rgb.Blue = (uint8_t)(z + m);
  }
  else if (hsv.Hue < 240.0f)
  {
    rgb.Red = (uint8_t)m;
    rgb.Green = (uint8_t)(z + m);
    rgb.Blue = (uint8_t)M;
  }
  else if (hsv.Hue < 300.0f)
  {
    rgb.Red = (uint8_t)(z + m);
    rgb.Green = (uint8_t)m;
    rgb.Blue = (uint8_t)M;
  }
  else
  {
    rgb.Red = (uint8_t)M;
    rgb.Green = (uint8_t)m;
    rgb.Blue = (uint8_t)(z + m);
  }

  return rgb;
}

SHsv GetHsvFromRgb(SRgb rgb)
{
  SHsv hsv = {0};

  const uint8_t r = rgb.Red;
  const uint8_t g = rgb.Green;
  const uint8_t b = rgb.Blue;

  float M = (float)Max(r, Max(g, b));
  float m = (float)Min(r, Min(g, b));
  
  hsv.Value = M / 255.0f;
  hsv.Saturation = (M == 0.0f) ? 0.0f : (1.0f - (m/M) );

  float x = radiansToDegrees( acosf( (r - (0.5f * g) - (0.5f * b) ) / (sqrtf(r*r + g*g + b*b - r*g - r*b - g*b) ) ) );
  hsv.Hue = (g >= b) ? x : (360.0f - x);
  return hsv;
}