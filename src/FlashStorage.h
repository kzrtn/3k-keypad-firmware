// Great souce for flash storage: https://kevinboone.me/picoflash.html?i=1

#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include <hardware/flash.h>
#include <hardware/sync.h>

typedef struct LedConfiguration
{
  uint8_t SwitchLedColor[3];
  uint8_t UnderglowLedColor[2];
  // Add more config stuff here
} SLedConfiguration;

void WriteLedConfigToFlash(SLedConfiguration ledConfig);
SLedConfiguration ReadLedConfigFromFlash();

#endif