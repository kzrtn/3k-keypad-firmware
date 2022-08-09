// Great souce for flash storage: https://kevinboone.me/picoflash.html?i=1

#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include "StateMachine.h"
#include <hardware/flash.h>
#include <hardware/sync.h>

typedef struct LedConfiguration
{
  uint32_t SwitchLedColor[3];
  uint32_t UnderglowLedColor[2];
  EMode SwitchLedMode;
  EMode UnderglowLedMode;
  // Add more config stuff here
} SLedConfiguration;

void WriteLedConfigToFlash(SLedConfiguration ledConfig);
SLedConfiguration ReadLedConfigFromFlash();

#endif