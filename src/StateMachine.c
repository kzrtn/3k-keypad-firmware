#include "StateMachine.h"
#include "EventGenerator.h"
#include "FlashStorage.h"
#include "pico/stdlib.h"
#include "math.h"

void sw_put_pixel(uint32_t pixel_grb);
void u_put_pixel(uint32_t pixel_grb);
uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);

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

typedef enum State
{
  SelectLedGroup,
  UnderglowLedMode,
  UnderglowLedHue,
  UnderglowLedSaturation,
  UnderglowLedValue,
  SwitchLedMode,
  SwitchLedHue,
  SwitchLedSaturation,
  SwitchLedValue
} EState;

static EState state = SelectLedGroup;

bool Handle_SelectLedGroup();
bool Handle_UnderglowLedMode();
bool Handle_UnderglowLedHue();
bool Handle_UnderglowLedSaturation();
bool Handle_UnderglowLedValue();
bool Handle_SwitchLedMode();
bool Handle_SwitchLedHue();
bool Handle_SwitchLedSaturation();
bool Handle_SwitchLedValue();

void ShowLedConfig();
void SaveLedConfig();

SRgb GetRgbFromHsv(SHsv hsv);
SHsv GetHsvFromRgb(SRgb rgb);

inline static uint8_t Min(uint8_t val1, uint8_t val2)
{
  return (val1 < val2) ? val1 : val2;
}

inline static uint8_t Max(uint8_t val1, uint8_t val2)
{
  return (val1 > val2) ? val1 : val2;
}

static inline float degreesToRadians(float degrees)
{ 
  return (degrees * 0.0174532925f);
}

static SLedConfiguration newLedConfig = {0};
bool configurationRead = false;

static SRgb currentSwitchRgb = {0};
static SRgb currentUnderglowRgb = {0};
static SHsv currentSwitchHsv = {0};
static SHsv currentUnderglowHsv = {0};

bool HandleStateMachine()
{
  // Look for new events to handle
  UpdateEventGenerator();
  
  switch (state)
  {
  case SelectLedGroup:
    return Handle_SelectLedGroup();
  case UnderglowLedMode:
    return Handle_UnderglowLedMode();
  case UnderglowLedHue:
    return Handle_UnderglowLedHue();
  case UnderglowLedSaturation:
    return Handle_UnderglowLedSaturation();
  case UnderglowLedValue:
    return Handle_UnderglowLedValue();
  case SwitchLedMode:
    return Handle_SwitchLedMode();
  case SwitchLedHue:
    return Handle_SwitchLedHue();
  case SwitchLedSaturation:
    return Handle_SwitchLedSaturation();
  case SwitchLedValue:
    return Handle_SwitchLedValue();

  default:
    return false; // Should never reach this part!
  }
}

bool Handle_SelectLedGroup()
{
  SEvent event = GetEvent();

  // Do state stuff

  if (!configurationRead) // Read configuration only once
  {
    newLedConfig = ReadLedConfigFromFlash();
    currentSwitchRgb.Data = newLedConfig.SwitchLedColor[0];
    currentUnderglowRgb.Data = newLedConfig.UnderglowLedColor[0];
    currentSwitchHsv = GetHsvFromRgb(currentSwitchRgb);
    currentUnderglowHsv = GetHsvFromRgb(currentUnderglowRgb);
    configurationRead = true;
  }

  ShowLedConfig();

  // Check for event
  if (event.KeyPressed[0])
  {
    state = UnderglowLedMode;
  }
  else if (event.KeyPressed[1])
  {
    state = SwitchLedMode;
  }
  else if (event.KeyPressed[2])
  {
    SaveLedConfig();
    return false; // Exit state machine
  }

  return true;
}

bool Handle_UnderglowLedMode()
{
  SEvent event = GetEvent();

  // Do state stuff
  ShowLedConfig();

  // Check for event
  if (event.KeyPressed[2])
  {
    state = UnderglowLedHue;
  }

  return true;
}

bool Handle_UnderglowLedHue()
{
  SEvent event = GetEvent();

  // Do state stuff
  if (event.KeyPressed[0])
  {
    float newHue = currentUnderglowHsv.Hue - 5.0f;
    currentUnderglowHsv.Hue = (newHue < 0.0f) ? 360.0f - newHue : newHue;
    currentUnderglowRgb = GetRgbFromHsv(currentUnderglowHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentUnderglowHsv.Hue = fmod(currentUnderglowHsv.Hue + 5.0f, 360.0f);
    currentUnderglowRgb = GetRgbFromHsv(currentUnderglowHsv);
  }

  ShowLedConfig();
  
  // Check for event
  if (event.KeyPressed[2])
  {
    state = UnderglowLedSaturation;
  }

  return true;
}

bool Handle_UnderglowLedSaturation()
{
  SEvent event = GetEvent();

  // Do state stuff
  if (event.KeyPressed[0])
  {
    currentUnderglowHsv.Saturation -= 0.1f;

    if (currentUnderglowHsv.Saturation < 0.0f)
      currentUnderglowHsv.Saturation = 0.0f;

    currentUnderglowRgb = GetRgbFromHsv(currentUnderglowHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentUnderglowHsv.Saturation += 0.1f;

    if (currentUnderglowHsv.Saturation > 1.0f)
      currentUnderglowHsv.Saturation = 1.0f;

    currentUnderglowRgb = GetRgbFromHsv(currentUnderglowHsv);
  }

  ShowLedConfig();

  // Check for event
  if (event.KeyPressed[2])
  {
    state = UnderglowLedValue;
  }

  return true;
}

bool Handle_UnderglowLedValue()
{
  SEvent event = GetEvent();

  // Do state stuff
  if (event.KeyPressed[0])
  {
    currentUnderglowHsv.Value -= 0.1f;

    if (currentUnderglowHsv.Value < 0.0f)
      currentUnderglowHsv.Value = 0.0f;

    currentUnderglowRgb = GetRgbFromHsv(currentUnderglowHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentUnderglowHsv.Value += 0.1f;

    if (currentUnderglowHsv.Value > 1.0f)
      currentUnderglowHsv.Value = 1.0f;

    currentUnderglowRgb = GetRgbFromHsv(currentUnderglowHsv);
  }

  ShowLedConfig();

  // Check for event
  if (event.KeyPressed[2])
  {
    SaveLedConfig();
    return false; // Exit state machine
  }

  return true;
}

bool Handle_SwitchLedMode()
{
  SEvent event = GetEvent();

  // Do state stuff
  ShowLedConfig();

  // Check for event
  if (event.KeyPressed[2])
  {
    state = SwitchLedHue;
  }

  return true;
}

bool Handle_SwitchLedHue()
{
  SEvent event = GetEvent();

  // Do state stuff
  if (event.KeyPressed[0])
  {
    float newHue = currentSwitchHsv.Hue - 5.0f;
    currentSwitchHsv.Hue = (newHue < 0.0f) ? 360.0f - newHue : newHue;
    currentSwitchRgb = GetRgbFromHsv(currentSwitchHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentSwitchHsv.Hue = fmod(currentSwitchHsv.Hue + 5.0f, 360.0f);
    currentSwitchRgb = GetRgbFromHsv(currentSwitchHsv);
  }

  ShowLedConfig();

  // Check for event
  if (event.KeyPressed[2])
  {
    state = SwitchLedSaturation;
  }

  return true;
}

bool Handle_SwitchLedSaturation()
{
  SEvent event = GetEvent();

  // Do state stuff
  if (event.KeyPressed[0])
  {
    currentSwitchHsv.Saturation -= 0.1f;

    if (currentSwitchHsv.Saturation < 0.0f)
      currentSwitchHsv.Saturation = 0.0f;

    currentSwitchRgb = GetRgbFromHsv(currentSwitchHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentSwitchHsv.Saturation += 0.1f;

    if (currentSwitchHsv.Saturation > 1.0f)
      currentSwitchHsv.Saturation = 1.0f;

    currentSwitchRgb = GetRgbFromHsv(currentSwitchHsv);
  }

  ShowLedConfig();

  // Check for event
  if (event.KeyPressed[2])
  {
    state = SwitchLedValue;
  }

  return true;
}

bool Handle_SwitchLedValue()
{
  SEvent event = GetEvent();

  // Do state stuff
  if (event.KeyPressed[0])
  {
    currentSwitchHsv.Value -= 0.1f;

    if (currentSwitchHsv.Value < 0.0f)
      currentSwitchHsv.Value = 0.0f;

    currentSwitchRgb = GetRgbFromHsv(currentSwitchHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentSwitchHsv.Value += 0.1f;

    if (currentSwitchHsv.Value > 1.0f)
      currentSwitchHsv.Value = 1.0f;

    currentSwitchRgb = GetRgbFromHsv(currentSwitchHsv);
  }

  ShowLedConfig();

  // Check for event
  if (event.KeyPressed[2])
  {
    SaveLedConfig();
    return false; // Exit state machine
  }

  return true;
}

void ShowLedConfig()
{
  sw_put_pixel(currentSwitchRgb.Data);
  sw_put_pixel(currentSwitchRgb.Data);
  sw_put_pixel(currentSwitchRgb.Data);
  u_put_pixel(currentUnderglowRgb.Data);
  u_put_pixel(currentUnderglowRgb.Data);
  sleep_ms(1);
}

void SaveLedConfig()
{
  newLedConfig.SwitchLedColor[0] = currentSwitchRgb.Data;
  newLedConfig.SwitchLedColor[1] = currentSwitchRgb.Data;
  newLedConfig.SwitchLedColor[2] = currentSwitchRgb.Data;
  newLedConfig.UnderglowLedColor[0] = currentUnderglowRgb.Data;
  newLedConfig.UnderglowLedColor[1] = currentUnderglowRgb.Data;
  newLedConfig.UnderglowLedColor[2] = currentUnderglowRgb.Data;
  WriteLedConfigToFlash(newLedConfig);
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
  hsv.Saturation = (M == 0.0f) ? 0 : (1.0f - (m/M) );

  float x = acosf( degreesToRadians( (r - (0.5f * g) - (0.5f * b) ) / (sqrtf(r*r + g*g + b*b - r*g - r*b - g*b) ) ) );
  hsv.Hue = (g >= b) ? x : (360.0f - x);
  return hsv;
}