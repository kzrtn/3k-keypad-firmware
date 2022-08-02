#include "StateMachine.h"
#include "EventGenerator.h"
#include "FlashStorage.h"
#include "pico/stdlib.h"

void sw_put_pixel(uint32_t pixel_grb);
void u_put_pixel(uint32_t pixel_grb);
uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);

typedef enum State
{
  ChooseLed,
  UnderglowConfig,
  SwitchLedConfig
} EState;

static EState state = ChooseLed;

bool Handle_ChooseLed();
bool Handle_UnderglowConfig();
bool Handle_SwitchLedConfig();
void SaveLEdConfig();

static SLedConfiguration newLedConfig = {0};

bool HandleStateMachine()
{
  // Look for new events to handle
  UpdateEventGenerator();
  
  switch (state)
  {
  case ChooseLed:
    return Handle_ChooseLed();
  case UnderglowConfig:
    return Handle_UnderglowConfig();
  case SwitchLedConfig:
    return Handle_SwitchLedConfig();
  default:
    return false; // Should never reach this part!
  }
}

bool Handle_ChooseLed()
{
  // Do state stuff
  sw_put_pixel(urgb_u32(100, 0, 0));
  sw_put_pixel(urgb_u32(100, 0, 0));
  sw_put_pixel(urgb_u32(100, 0, 0));
  u_put_pixel(urgb_u32(100, 0, 0));
  u_put_pixel(urgb_u32(100, 0, 0));
  sleep_ms(1);

  // Check for event
  SEvent event = GetEvent();
  if (event.KeyPressed[0])
  {
    state = UnderglowConfig;
  }
  else if (event.KeyPressed[1])
  {
    state = SwitchLedConfig;
  }
  else if (event.KeyPressed[2])
  {
    newLedConfig.SwitchLedColor[0] = urgb_u32(100, 0, 0);
    newLedConfig.SwitchLedColor[1] = urgb_u32(100, 0, 0);
    newLedConfig.SwitchLedColor[2] = urgb_u32(100, 0, 0);
    newLedConfig.UnderglowLedColor[0] = urgb_u32(100, 0, 0);
    newLedConfig.UnderglowLedColor[1] = urgb_u32(100, 0, 0);
    SaveLedConfig();
    return false; // Exit state machine
  }

  return true;
}

bool Handle_UnderglowConfig()
{
  // Do state stuff
  sw_put_pixel(urgb_u32(0, 100, 0));
  sw_put_pixel(urgb_u32(0, 100, 0));
  sw_put_pixel(urgb_u32(0, 100, 0));
  u_put_pixel(urgb_u32(0, 100, 0));
  u_put_pixel(urgb_u32(0, 100, 0));
  sleep_ms(1);

  // Check for event
  SEvent event = GetEvent();
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

bool Handle_SwitchLedConfig()
{
  // Do state stuff
  sw_put_pixel(urgb_u32(0, 0, 100));
  sw_put_pixel(urgb_u32(0, 0, 100));
  sw_put_pixel(urgb_u32(0, 0, 100));
  u_put_pixel(urgb_u32(0, 0, 100));
  u_put_pixel(urgb_u32(0, 0, 100));
  sleep_ms(1);

  // Check for event
  SEvent event = GetEvent();
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

void SaveLedConfig()
{
  WriteLedConfigToFlash(newLedConfig);
  // TODO: IS a delay needed here to wait for the memory to be written?
}