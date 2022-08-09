// Great souce for flash storage: https://kevinboone.me/picoflash.html?i=1

#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include "StateMachine.h"
#include <hardware/flash.h>
#include <hardware/sync.h>

typedef struct LedConfiguration
{
  EMode SwitchLedMode;
  EMode UnderglowLedMode;

  // ==================== For static and reactive mode
  uint32_t SwitchLedColor;
  uint32_t UnderglowLedColor;

  // ==================== For rgb cycle and fade mode
  uint32_t SwitchLedSpeed;
  float SwitchLedSaturation;
  float SwitchLedValue;

  uint32_t UnderglowLedSpeed;
  float UnderglowLedSaturation;
  float UnderglowLedValue;

  // Add more config stuff here
} SLedConfiguration;

void WriteLedConfigToFlash(SLedConfiguration ledConfig);
SLedConfiguration ReadLedConfigFromFlash();

#endif