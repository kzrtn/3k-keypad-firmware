#include "StateMachine.h"
#include "EventGenerator.h"
#include "FlashStorage.h"
#include "ColorConversion.h"
#include "pico/stdlib.h"

void sw_put_pixel(uint32_t pixel_grb);  // TODO: To be removed when these are put in their own file
void u_put_pixel(uint32_t pixel_grb);
uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);

const float HueStepSize = 1.0f;
const float SaturationStepSize = 0.02f;
const float ValueStepSize = 0.02f;
const uint32_t SpeedStepSize = 10;

const uint32_t SpeedMin = 10;
const uint32_t SpeedMax = 1000;

typedef enum State
{
  SelectLedGroup,
  UnderglowLedMode,
  UnderglowLedHue,
  UnderglowLedHueSpeed,
  UnderglowLedSaturation,
  UnderglowLedValue,
  SwitchLedMode,
  SwitchLedHue,
  SwitchLedHueSpeed,
  SwitchLedSaturation,
  SwitchLedValue
} EState;

static EState state = SelectLedGroup;

bool Handle_SelectLedGroup();
bool Handle_UnderglowLedMode();
bool Handle_UnderglowLedHue();
bool Handle_UnderglowLedHueSpeed();
bool Handle_UnderglowLedSaturation();
bool Handle_UnderglowLedValue();
bool Handle_SwitchLedMode();
bool Handle_SwitchLedHue();
bool Handle_SwitchLedHueSpeed();
bool Handle_SwitchLedSaturation();
bool Handle_SwitchLedValue();

void LoadLedConfig();
void ShowLedConfig();
void SaveLedConfig();
void ShowUnderglowLedModeSelection();
void ShowSwitchLedModeSelection();

static SLedConfiguration newLedConfig = {0};
bool configurationRead = false;

static EMode currentSwitchMode = 0;
static EMode currentUnderglowMode = 0;
// For static and reactive mode
static SRgb currentSwitchRgb = {0};
static SRgb currentUnderglowRgb = {0};
static SHsv currentSwitchHsv = {0};
static SHsv currentUnderglowHsv = {0};
// For rgb cycle and fade mode
static uint32_t currentSwitchLedSpeed = 0;
static uint32_t currentUnderglowLedSpeed = 0;
static float currentSwitchLedSaturation = 0.0f;
static float currentUnderglowLedSaturation = 0.0f;
static float currentSwitchLedValue = 0.0f;
static float currentUnderglowLedValue = 0.0f;

float BoundsWrapAround(float input, float lowerBound, float upperBound)
{
  if (input < lowerBound)
    return upperBound - (lowerBound - input) + 1.0f;
  
  if (input > upperBound)
    return lowerBound + (input - upperBound) - 1.0f;
  
  return input;
}

float BoundsClamp(float input, float lowerBound, float upperBound)
{
  const float min = (input < lowerBound) ? lowerBound : input;
  return (min > upperBound) ? upperBound : min;
}

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
    LoadLedConfig();
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
  if (event.KeyPressed[0])
  {
    currentUnderglowMode = BoundsWrapAround(currentUnderglowMode - 1, Mode_Static, Mode_RgbFade);
  }
  else if (event.KeyPressed[1])
  {
    currentUnderglowMode = BoundsWrapAround(currentUnderglowMode + 1, Mode_Static, Mode_RgbFade);
  }

  ShowUnderglowLedModeSelection();

  // Check for event
  if (event.KeyPressed[2])
  {
    if (currentUnderglowMode == Mode_Static)
    {
      state = UnderglowLedHue;
    }
    else if ( currentUnderglowMode == Mode_RgbCycle ||
              currentUnderglowMode == Mode_RgbFade)
    {
      state = UnderglowLedHueSpeed;
    }
    else
    {
      return false; // Non-valid mode
    }  
  }

  return true;
}

bool Handle_UnderglowLedHue()
{
  SEvent event = GetEvent();

  // Do state stuff
  if (event.KeyPressed[0])
  {
    currentUnderglowHsv.Hue = BoundsWrapAround(currentUnderglowHsv.Hue - HueStepSize, 0.0f, 360.0f);
    currentUnderglowRgb = GetRgbFromHsv(currentUnderglowHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentUnderglowHsv.Hue = BoundsWrapAround(currentUnderglowHsv.Hue + HueStepSize, 0.0f, 360.0f);
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

bool Handle_UnderglowLedHueSpeed()
{
  SEvent event = GetEvent();

  // Do state stuff
  if (event.KeyPressed[0])
  {
    currentUnderglowLedSpeed = BoundsClamp(currentUnderglowLedSpeed - SpeedStepSize, SpeedMin, SpeedMax);
  }
  else if (event.KeyPressed[1])
  {
    currentUnderglowLedSpeed = BoundsClamp(currentUnderglowLedSpeed + SpeedStepSize, SpeedMin, SpeedMax);
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
    currentUnderglowHsv.Saturation = BoundsClamp(currentUnderglowHsv.Saturation - SaturationStepSize, 0.0f, 1.0f);
    currentUnderglowLedSaturation = currentUnderglowHsv.Saturation;
    currentUnderglowRgb = GetRgbFromHsv(currentUnderglowHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentUnderglowHsv.Saturation = BoundsClamp(currentUnderglowHsv.Saturation + SaturationStepSize, 0.0f, 1.0f);
    currentUnderglowLedSaturation = currentUnderglowHsv.Saturation;
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
    currentUnderglowHsv.Value = BoundsClamp(currentUnderglowHsv.Value - ValueStepSize, 0.0f, 1.0f);
    currentUnderglowLedValue = currentUnderglowHsv.Value;
    currentUnderglowRgb = GetRgbFromHsv(currentUnderglowHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentUnderglowHsv.Value = BoundsClamp(currentUnderglowHsv.Value + ValueStepSize, 0.0f, 1.0f);
    currentUnderglowLedValue = currentUnderglowHsv.Value;
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
  if (event.KeyPressed[0])
  {
    currentSwitchMode = BoundsWrapAround(currentSwitchMode - 1, Mode_Static, Mode_ReactiveInverse);
  }
  else if (event.KeyPressed[1])
  {
    currentSwitchMode = BoundsWrapAround(currentSwitchMode + 1, Mode_Static, Mode_ReactiveInverse);
  }

  ShowSwitchLedModeSelection();

  // Check for event
  if (event.KeyPressed[2])
  {
    if (currentSwitchMode == Mode_Static ||
        currentSwitchMode == Mode_Reactive ||
        currentSwitchMode == Mode_ReactiveInverse)
    {
      state = SwitchLedHue;
    }
    else if ( currentSwitchMode == Mode_RgbCycle ||
              currentSwitchMode == Mode_RgbFade)
    {
      state = SwitchLedHueSpeed;
    }
    else
    {
      return false; // Non-valid mode
    } 
  }

  return true;
}

bool Handle_SwitchLedHue()
{
  SEvent event = GetEvent();

  // Do state stuff
  if (event.KeyPressed[0])
  {
    currentSwitchHsv.Hue = BoundsWrapAround(currentSwitchHsv.Hue - HueStepSize, 0.0f, 360.0f);
    currentSwitchRgb = GetRgbFromHsv(currentSwitchHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentSwitchHsv.Hue = BoundsWrapAround(currentSwitchHsv.Hue + HueStepSize, 0.0f, 360.0f);
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

bool Handle_SwitchLedHueSpeed()
{
  SEvent event = GetEvent();

  // Do state stuff
  if (event.KeyPressed[0])
  {
    currentSwitchLedSpeed = BoundsClamp(currentSwitchLedSpeed - SpeedStepSize, SpeedMin, SpeedMax);
  }
  else if (event.KeyPressed[1])
  {
    currentSwitchLedSpeed = BoundsClamp(currentSwitchLedSpeed + SpeedStepSize, SpeedMin, SpeedMax);
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
    currentSwitchHsv.Saturation = BoundsClamp(currentSwitchHsv.Saturation - SaturationStepSize, 0.0f, 1.0f);
    currentSwitchLedSaturation = currentSwitchHsv.Saturation;
    currentSwitchRgb = GetRgbFromHsv(currentSwitchHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentSwitchHsv.Saturation = BoundsClamp(currentSwitchHsv.Saturation + SaturationStepSize, 0.0f, 1.0f);
    currentSwitchLedSaturation = currentSwitchHsv.Saturation;
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
    currentSwitchHsv.Value = BoundsClamp(currentSwitchHsv.Value - ValueStepSize, 0.0f, 1.0f);
    currentSwitchLedValue = currentSwitchHsv.Value;
    currentSwitchRgb = GetRgbFromHsv(currentSwitchHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentSwitchHsv.Value = BoundsClamp(currentSwitchHsv.Value + ValueStepSize, 0.0f, 1.0f);
    currentSwitchLedValue = currentSwitchHsv.Value;
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

void LoadLedConfig()
{
  newLedConfig = ReadLedConfigFromFlash();

  currentSwitchMode = newLedConfig.SwitchLedMode;
  currentUnderglowMode = newLedConfig.UnderglowLedMode;

  // When retrieving uninitialized flash the mode can be undefined. 
  // In that case default to known values
  if (currentSwitchMode > Mode_ReactiveInverse) // With a non-valid mode default to reactive for switches
    currentSwitchMode = Mode_Reactive;

  if (currentUnderglowMode > Mode_RgbFade) // With a non-valid mode default to reactive for switches
    currentUnderglowMode = Mode_Static; 

  // For static and reactive mode 
  currentSwitchRgb.Data = newLedConfig.SwitchLedColor;
  currentSwitchHsv = GetHsvFromRgb(currentSwitchRgb);

  currentUnderglowRgb.Data = newLedConfig.UnderglowLedColor;
  currentUnderglowHsv = GetHsvFromRgb(currentUnderglowRgb);
  
  // For rgb cycle and fade mode
  currentSwitchLedSpeed = newLedConfig.SwitchLedSpeed;
  currentSwitchLedSaturation = newLedConfig.SwitchLedSaturation;
  currentSwitchLedValue = newLedConfig.SwitchLedValue;

  currentUnderglowLedSpeed = newLedConfig.UnderglowLedSpeed;
  currentUnderglowLedSaturation = newLedConfig.UnderglowLedSaturation;
  currentUnderglowLedValue = newLedConfig.UnderglowLedValue;  
}

void ShowLedConfig()
{
  // TODO: Expand to show animations if selected

  sw_put_pixel(currentSwitchRgb.Data);
  sw_put_pixel(currentSwitchRgb.Data);
  sw_put_pixel(currentSwitchRgb.Data);
  u_put_pixel(currentUnderglowRgb.Data);
  u_put_pixel(currentUnderglowRgb.Data);
  sleep_ms(1);
}

void SaveLedConfig()
{
  newLedConfig.SwitchLedMode = currentSwitchMode;
  newLedConfig.UnderglowLedMode = currentSwitchMode;

  // For static and reactive mode
  newLedConfig.SwitchLedColor = currentSwitchRgb.Data;
  newLedConfig.UnderglowLedColor = currentUnderglowRgb.Data;
  
  // For rgb cycle and fade mode
  newLedConfig.SwitchLedSpeed = currentSwitchLedSpeed;
  newLedConfig.SwitchLedSaturation = currentSwitchLedSaturation;
  newLedConfig.SwitchLedValue = currentSwitchLedValue;

  newLedConfig.UnderglowLedSpeed = currentUnderglowLedSpeed;
  newLedConfig.UnderglowLedSaturation = currentUnderglowLedSaturation;
  newLedConfig.UnderglowLedValue = currentUnderglowLedValue;

  WriteLedConfigToFlash(newLedConfig);
}

void ShowUnderglowLedModeSelection()
{
  switch (currentUnderglowMode)
  {
  case Mode_Static:     
    sw_put_pixel(urgb_u32(0, 100, 0));
    sw_put_pixel(0);
    sw_put_pixel(0);
    break;
  case Mode_RgbCycle:
    // TODO: Add animation
    sw_put_pixel(0);
    sw_put_pixel(0);
    sw_put_pixel(0);
    break;
  case Mode_RgbFade:
    // TODO: Add animation
    sw_put_pixel(0);
    sw_put_pixel(0);
    sw_put_pixel(0);
    break;
  
  default:
    break;
  }

  sleep_ms(1);
}

void ShowSwitchLedModeSelection()
{
  switch (currentSwitchMode)
  {
  case Mode_Static: 
    sw_put_pixel(0);
    sw_put_pixel(urgb_u32(0, 100, 0));
    sw_put_pixel(0);
    break;
  case Mode_RgbCycle:
    // TODO: Add animation
    sw_put_pixel(0);
    sw_put_pixel(0);
    sw_put_pixel(0);
    break;
  case Mode_RgbFade:
    // TODO: Add animation
    sw_put_pixel(0);
    sw_put_pixel(0);
    sw_put_pixel(0);
    break;
  case Mode_Reactive:
    sw_put_pixel(0);
    sw_put_pixel(urgb_u32(100, 0, 0)); // Red
    sw_put_pixel(0);
    break;
  case Mode_ReactiveInverse:
    sw_put_pixel(0);
    sw_put_pixel(urgb_u32(0, 0, 100)); // Blue
    sw_put_pixel(0);
    break;
  
  default:
    break;
  }

  sleep_ms(1);
}