#include "FlashStorage.h"
#include <string.h>

#define LED_CONFIG_SIZE                 (sizeof(SLedConfiguration))
#define NR_OF_SECTORS                   ((LED_CONFIG_SIZE % FLASH_SECTOR_SIZE > 0) ? LED_CONFIG_SIZE / FLASH_SECTOR_SIZE + 1 : LED_CONFIG_SIZE / FLASH_SECTOR_SIZE)
#define CONFIG_BLOCK_SIZE_SECTOR        (FLASH_SECTOR_SIZE * NR_OF_SECTORS)

#define PICO_FLASH_SIZE                 (0x200000) // 2MB

#define END_OF_FLASH_ADDR_SPACE_SYSTEM  (XIP_BASE + PICO_FLASH_SIZE)  // For read
#define END_OF_FLASH_ADDR_SPACE_FLASH   (PICO_FLASH_SIZE)             // For write and clear

#define CONFIG_BLOCK_START_READ         (END_OF_FLASH_ADDR_SPACE_SYSTEM - CONFIG_BLOCK_SIZE_SECTOR)
#define CONFIG_BLOCK_START_WRITE        (END_OF_FLASH_ADDR_SPACE_FLASH - CONFIG_BLOCK_SIZE_SECTOR)
#define CONFIG_BLOCK_START_CLEAR        (END_OF_FLASH_ADDR_SPACE_FLASH - CONFIG_BLOCK_SIZE_SECTOR)

void WriteLedConfigToFlash(SLedConfiguration ledConfig)
{
  uint8_t buffer[CONFIG_BLOCK_SIZE_SECTOR] = {0};
  
  // Copy the struct to a buffer
  memcpy(buffer, (uint8_t*)&ledConfig, LED_CONFIG_SIZE); 
  
  // TODO: If we go multicore we will probably need a mutex here
  // Disable interrupts while writing to flash
  uint32_t interrupts = save_and_disable_interrupts();

  // Erase memory before writing
  flash_range_erase(CONFIG_BLOCK_START_CLEAR, CONFIG_BLOCK_SIZE_SECTOR);

  // Write the minimum number of flash pages at the end of flash address space
  flash_range_program(CONFIG_BLOCK_START_WRITE, buffer, CONFIG_BLOCK_SIZE_SECTOR);
  
  // Re-enable the interrupts
  restore_interrupts(interrupts);
}

SLedConfiguration ReadLedConfigFromFlash()
{
  SLedConfiguration ledConfig = {0};

  // Copy the contents of flash to ledConfig
  memcpy((uint8_t*)&ledConfig, (uint8_t*)CONFIG_BLOCK_START_READ, LED_CONFIG_SIZE);

  return ledConfig;
}