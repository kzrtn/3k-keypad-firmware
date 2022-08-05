#include "StateMachine.h"
#include "EventGenerator.h"
#include "FlashStorage.h"
#include "pico/stdlib.h"

void sw_put_pixel(uint32_t pixel_grb);
void u_put_pixel(uint32_t pixel_grb);
uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);

typedef enum State
{
  SelectLedGroup,
  UnderglowLedMode,
  UnderglowLedHue,
  UnderglowLedSaturation,
  UnderglowLedBrightness,
  SwitchLedMode,
  SwitchLedHue,
  SwitchLedSaturation,
  SwitchLedBrightness
} EState;

static EState state = SelectLedGroup;

bool Handle_SelectLedGroup();
bool Handle_UnderglowLedMode();
bool Handle_UnderglowLedHue();
bool Handle_UnderglowLedSaturation();
bool Handle_UnderglowLedBrightness();
bool Handle_SwitchLedMode();
bool Handle_SwitchLedHue();
bool Handle_SwitchLedSaturation();
bool Handle_SwitchLedBrightness();
void SaveLedConfig();

static SLedConfiguration newLedConfig = {0};
bool configurationRead = false;

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
  case UnderglowLedBrightness:
    return Handle_UnderglowLedBrightness();
  case SwitchLedMode:
    return Handle_SwitchLedMode();
  case SwitchLedHue:
    return Handle_SwitchLedHue();
  case SwitchLedSaturation:
    return Handle_SwitchLedSaturation();
  case SwitchLedBrightness:
    return Handle_SwitchLedBrightness();

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
    configurationRead = true;
  }

  sw_put_pixel(urgb_u32(100, 0, 0));
  sw_put_pixel(urgb_u32(100, 0, 0));
  sw_put_pixel(urgb_u32(100, 0, 0));
  u_put_pixel(urgb_u32(100, 0, 0));
  u_put_pixel(urgb_u32(100, 0, 0));
  sleep_ms(1);

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
  sw_put_pixel(urgb_u32(0, 0, 100));
  sw_put_pixel(urgb_u32(0, 0, 100));
  sw_put_pixel(urgb_u32(0, 0, 100));
  u_put_pixel(urgb_u32(0, 0, 100));
  u_put_pixel(urgb_u32(0, 0, 100));
  sleep_ms(1);

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
  sw_put_pixel(urgb_u32(0, 0, 100));
  sw_put_pixel(urgb_u32(0, 0, 0));
  sw_put_pixel(urgb_u32(0, 0, 0));
  u_put_pixel(urgb_u32(0, 0, 100));
  u_put_pixel(urgb_u32(0, 0, 100));
  sleep_ms(1);

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
  sw_put_pixel(urgb_u32(0, 0, 0));
  sw_put_pixel(urgb_u32(0, 0, 100));
  sw_put_pixel(urgb_u32(0, 0, 0));
  u_put_pixel(urgb_u32(0, 0, 100));
  u_put_pixel(urgb_u32(0, 0, 100));
  sleep_ms(1);

  // Check for event
  if (event.KeyPressed[2])
  {
    state = UnderglowLedBrightness;
  }

  return true;
}

bool Handle_UnderglowLedBrightness()
{
  SEvent event = GetEvent();

  // Do state stuff
  sw_put_pixel(urgb_u32(0, 0, 0));
  sw_put_pixel(urgb_u32(0, 0, 0));
  sw_put_pixel(urgb_u32(0, 0, 100));
  u_put_pixel(urgb_u32(0, 0, 100));
  u_put_pixel(urgb_u32(0, 0, 100));
  sleep_ms(1);

  // Check for event
  if (event.KeyPressed[2])
  {
    newLedConfig.SwitchLedColor[0] = urgb_u32(0, 0, 100);
    newLedConfig.SwitchLedColor[1] = urgb_u32(0, 0, 100);
    newLedConfig.SwitchLedColor[2] = urgb_u32(0, 0, 100);
    newLedConfig.UnderglowLedColor[0] = urgb_u32(0, 0, 100);
    newLedConfig.UnderglowLedColor[1] = urgb_u32(0, 0, 100); 
    SaveLedConfig();
    return false; // Exit state machine
  }

  return true;
}

bool Handle_SwitchLedMode()
{
  SEvent event = GetEvent();

  // Do state stuff
  sw_put_pixel(urgb_u32(0, 100, 0));
  sw_put_pixel(urgb_u32(0, 100, 0));
  sw_put_pixel(urgb_u32(0, 100, 0));
  u_put_pixel(urgb_u32(0, 100, 0));
  u_put_pixel(urgb_u32(0, 100, 0));
  sleep_ms(1);

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
  sw_put_pixel(urgb_u32(0, 100, 0));
  sw_put_pixel(urgb_u32(0, 0, 0));
  sw_put_pixel(urgb_u32(0, 0, 0));
  u_put_pixel(urgb_u32(0, 100, 0));
  u_put_pixel(urgb_u32(0, 100, 0));
  sleep_ms(1);

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
  sw_put_pixel(urgb_u32(0, 0, 0));
  sw_put_pixel(urgb_u32(0, 100, 0));
  sw_put_pixel(urgb_u32(0, 0, 0));
  u_put_pixel(urgb_u32(0, 100, 0));
  u_put_pixel(urgb_u32(0, 100, 0));
  sleep_ms(1);

  // Check for event
  if (event.KeyPressed[2])
  {
    state = SwitchLedBrightness;
  }

  return true;
}

bool Handle_SwitchLedBrightness()
{
  SEvent event = GetEvent();

  // Do state stuff
  sw_put_pixel(urgb_u32(0, 0, 0));
  sw_put_pixel(urgb_u32(0, 0, 0));
  sw_put_pixel(urgb_u32(0, 100, 0));
  u_put_pixel(urgb_u32(0, 100, 0));
  u_put_pixel(urgb_u32(0, 100, 0));
  sleep_ms(1);

  // Check for event
  if (event.KeyPressed[2])
  {
    newLedConfig.SwitchLedColor[0] = urgb_u32(0, 100, 0);
    newLedConfig.SwitchLedColor[1] = urgb_u32(0, 100, 0);
    newLedConfig.SwitchLedColor[2] = urgb_u32(0, 100, 0);
    newLedConfig.UnderglowLedColor[0] = urgb_u32(0, 100, 0);
    newLedConfig.UnderglowLedColor[1] = urgb_u32(0, 100, 0); 
    SaveLedConfig();
    return false; // Exit state machine
  }

  return true;
}

void SaveLedConfig()
{
  WriteLedConfigToFlash(newLedConfig);
  // TODO: IS a delay needed here to wait for the memory to be written?
}