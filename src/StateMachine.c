#include "StateMachine.h"
#include "EventGenerator.h"
#include "FlashStorage.h"
#include "ColorConversion.h"
#include "Animations.h"
#include "pico/stdlib.h"

void sw_put_pixel(uint32_t pixel_grb);  // TODO: To be removed when these are put in their own file
void u_put_pixel(uint32_t pixel_grb);
uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);

const float HueStepSize = 1.0f;
const float SaturationStepSize = 0.02f;
const float ValueStepSize = 0.02f;
const float SpeedStepSize = 2.0f;

// Note: speed is the rate of change of the hue. It is expressed in degrees per second
const float SpeedMin = 0.0f;
const float SpeedMax = 240.0f; 

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
static uint64_t timestampUs = 0;

static EMode currentSwitchMode = 0;
static EMode currentUnderglowMode = 0;
// For static and reactive mode
static SRgb currentSwitchRgb = {0};
static SRgb currentUnderglowRgb = {0};
static SHsv currentSwitchHsv = {0};
static SHsv currentUnderglowHsv = {0};
// For rgb cycle and fade mode
static float currentSwitchLedAnimationSpeed = 0;
static float currentUnderglowLedAnimationSpeed = 0;
static SHsv currentSwitchLedAnimationColor = {0};
static SHsv currentUnderglowLedAnimationColor = {0};

static uint64_t previousSwitchAnimationUpdateTimeUs = 0;
static uint64_t previousUnderglowAnimationUpdateTimeUs = 0;

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
  case UnderglowLedHueSpeed:
    return Handle_UnderglowLedHueSpeed();
  case UnderglowLedSaturation:
    return Handle_UnderglowLedSaturation();
  case UnderglowLedValue:
    return Handle_UnderglowLedValue();
  case SwitchLedMode:
    return Handle_SwitchLedMode();
  case SwitchLedHue:
    return Handle_SwitchLedHue();
  case SwitchLedHueSpeed:
    return Handle_SwitchLedHueSpeed();
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
    currentUnderglowLedAnimationSpeed = BoundsClamp(currentUnderglowLedAnimationSpeed - SpeedStepSize, SpeedMin, SpeedMax);
  }
  else if (event.KeyPressed[1])
  {
    currentUnderglowLedAnimationSpeed = BoundsClamp(currentUnderglowLedAnimationSpeed + SpeedStepSize, SpeedMin, SpeedMax);
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
    currentUnderglowLedAnimationColor.Saturation = currentUnderglowHsv.Saturation;
    currentUnderglowRgb = GetRgbFromHsv(currentUnderglowHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentUnderglowHsv.Saturation = BoundsClamp(currentUnderglowHsv.Saturation + SaturationStepSize, 0.0f, 1.0f);
    currentUnderglowLedAnimationColor.Saturation = currentUnderglowHsv.Saturation;
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
    currentUnderglowLedAnimationColor.Value = currentUnderglowHsv.Value;
    currentUnderglowRgb = GetRgbFromHsv(currentUnderglowHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentUnderglowHsv.Value = BoundsClamp(currentUnderglowHsv.Value + ValueStepSize, 0.0f, 1.0f);
    currentUnderglowLedAnimationColor.Value = currentUnderglowHsv.Value;
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
    currentSwitchLedAnimationSpeed = BoundsClamp(currentSwitchLedAnimationSpeed - SpeedStepSize, SpeedMin, SpeedMax);
  }
  else if (event.KeyPressed[1])
  {
    currentSwitchLedAnimationSpeed = BoundsClamp(currentSwitchLedAnimationSpeed + SpeedStepSize, SpeedMin, SpeedMax);
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
    currentSwitchLedAnimationColor.Saturation = currentSwitchHsv.Saturation;
    currentSwitchRgb = GetRgbFromHsv(currentSwitchHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentSwitchHsv.Saturation = BoundsClamp(currentSwitchHsv.Saturation + SaturationStepSize, 0.0f, 1.0f);
    currentSwitchLedAnimationColor.Saturation = currentSwitchHsv.Saturation;
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
    currentSwitchLedAnimationColor.Value = currentSwitchHsv.Value;
    currentSwitchRgb = GetRgbFromHsv(currentSwitchHsv);
  }
  else if (event.KeyPressed[1])
  {
    currentSwitchHsv.Value = BoundsClamp(currentSwitchHsv.Value + ValueStepSize, 0.0f, 1.0f);
    currentSwitchLedAnimationColor.Value = currentSwitchHsv.Value;
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
  currentSwitchLedAnimationSpeed = newLedConfig.SwitchLedSpeed;
  currentSwitchLedAnimationColor.Hue = 0.0f;
  currentSwitchLedAnimationColor.Saturation = newLedConfig.SwitchLedSaturation;
  currentSwitchLedAnimationColor.Value = newLedConfig.SwitchLedValue;

  currentUnderglowLedAnimationSpeed = newLedConfig.UnderglowLedSpeed;
  currentUnderglowLedAnimationColor.Hue = 0.0f;
  currentUnderglowLedAnimationColor.Saturation = newLedConfig.UnderglowLedSaturation;
  currentUnderglowLedAnimationColor.Value = newLedConfig.UnderglowLedValue;  
}

void ShowLedConfig()
{
  // TODO: Expand to show animations if selected
  if (time_reached(timestampUs + 1000))
  {
    uint32_t rgb = 0;

    switch (currentSwitchMode)
    {
    case Mode_Static:
    case Mode_Reactive:
    case Mode_ReactiveInverse:
      sw_put_pixel(currentSwitchRgb.Data);
      sw_put_pixel(currentSwitchRgb.Data);
      sw_put_pixel(currentSwitchRgb.Data);
      break;
    case Mode_RgbCycle:
      rgb = GetColorRgbCycle(&previousSwitchAnimationUpdateTimeUs, currentSwitchLedAnimationSpeed, &currentSwitchLedAnimationColor).Data;
      sw_put_pixel(rgb);
      sw_put_pixel(rgb);
      sw_put_pixel(rgb);
      break;
    case Mode_RgbFade:
      rgb = GetColorRgbFade().Data;
      sw_put_pixel(rgb);
      sw_put_pixel(rgb);
      sw_put_pixel(rgb);
      break;
    
    default:
      break;
    }

    switch (currentUnderglowMode)
    {
    case Mode_Static:
      u_put_pixel(currentUnderglowRgb.Data);
      u_put_pixel(currentUnderglowRgb.Data);
      break;
    case Mode_RgbCycle:
      rgb = (uint32_t)GetColorRgbCycle(&previousUnderglowAnimationUpdateTimeUs, currentUnderglowLedAnimationSpeed, &currentUnderglowLedAnimationColor).Data;
      u_put_pixel(rgb);
      u_put_pixel(rgb);
      break;
    case Mode_RgbFade:
      rgb = GetColorRgbFade().Data;
      sw_put_pixel(rgb);
      sw_put_pixel(rgb);
      break;
    
    default:
      break;
    }

    timestampUs = time_us_64();
  }
}

void SaveLedConfig()
{
  newLedConfig.SwitchLedMode = currentSwitchMode;
  newLedConfig.UnderglowLedMode = currentUnderglowMode;

  // For static and reactive mode
  newLedConfig.SwitchLedColor = currentSwitchRgb.Data;
  newLedConfig.UnderglowLedColor = currentUnderglowRgb.Data;
  
  // For rgb cycle and fade mode
  newLedConfig.SwitchLedSpeed = currentSwitchLedAnimationSpeed;
  newLedConfig.SwitchLedSaturation = currentSwitchLedAnimationColor.Saturation;
  newLedConfig.SwitchLedValue = currentSwitchLedAnimationColor.Value;

  newLedConfig.UnderglowLedSpeed = currentUnderglowLedAnimationSpeed;
  newLedConfig.UnderglowLedSaturation = currentUnderglowLedAnimationColor.Saturation;
  newLedConfig.UnderglowLedValue = currentUnderglowLedAnimationColor.Value;

  WriteLedConfigToFlash(newLedConfig);
}

void ShowUnderglowLedModeSelection()
{
  if (time_reached(timestampUs + 1000))
  {
    timestampUs = time_us_64();

    switch (currentUnderglowMode)
    {
    case Mode_Static:     
      sw_put_pixel(urgb_u32(0, 100, 0)); // Green
      sw_put_pixel(0);
      sw_put_pixel(0);
      break;
    case Mode_RgbCycle:
      sw_put_pixel(GetColorRgbCycle(&previousUnderglowAnimationUpdateTimeUs, 90.0f, &currentUnderglowLedAnimationColor).Data);
      sw_put_pixel(0);
      sw_put_pixel(0);
      break;
    case Mode_RgbFade:
      sw_put_pixel(GetColorRgbFade().Data);
      sw_put_pixel(0);
      sw_put_pixel(0);
      break;
    
    default:
      break;
    }
  }
}

void ShowSwitchLedModeSelection()
{
  if (time_reached(timestampUs + 1000))
  {
    timestampUs = time_us_64();

    switch (currentSwitchMode)
    {
    case Mode_Static: 
      sw_put_pixel(0);
      sw_put_pixel(urgb_u32(0, 100, 0)); // Green
      sw_put_pixel(0);
      break;
    case Mode_RgbCycle:
      sw_put_pixel(0);
      sw_put_pixel(GetColorRgbCycle(&previousSwitchAnimationUpdateTimeUs, 90.0f, &currentSwitchLedAnimationColor).Data);
      sw_put_pixel(0);
      break;
    case Mode_RgbFade:
      sw_put_pixel(0);
      sw_put_pixel(GetColorRgbFade().Data);
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
  }
}