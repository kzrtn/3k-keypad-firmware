#include "FlashStorage.h"
#include <string.h>

#define LED_CONFIG_SIZE               (sizeof(SLedConfiguration))
#define NR_OF_PAGES                   ((LED_CONFIG_SIZE % FLASH_PAGE_SIZE > 0) ? LED_CONFIG_SIZE / FLASH_PAGE_SIZE + 1 : LED_CONFIG_SIZE / FLASH_PAGE_SIZE)
#define PICO_FLASH_SIZE               (0x200000) // 2MB
#define END_OF_FLASH_ADDR_SPACE_READ  (XIP_BASE + PICO_FLASH_SIZE)
#define END_OF_FLASH_ADDR_SPACE_WRITE (PICO_FLASH_SIZE)
#define CONFIG_BLOCK_SIZE             (FLASH_PAGE_SIZE * NR_OF_PAGES)
#define CONFIG_BLOCK_START_READ       (END_OF_FLASH_ADDR_SPACE_READ - CONFIG_BLOCK_SIZE)
#define CONFIG_BLOCK_START_WRITE      (END_OF_FLASH_ADDR_SPACE_WRITE - CONFIG_BLOCK_SIZE)

// You can only write to flash with a size that is a multiple of a page size (256 byte)
void WriteLedConfigToFlash(SLedConfiguration ledConfig)
{
  uint8_t buffer[CONFIG_BLOCK_SIZE] = {0};  
  
  // Copy the struct to a buffer
  memcpy(buffer, (uint8_t*)&ledConfig, LED_CONFIG_SIZE); 
  
  // TODO: If we go multicore we will probably need a mutex here
  // Disable interrupts while writing to flash
  uint32_t interrupts = save_and_disable_interrupts();

  // Write the minimum number of flash pages at the end of flash address space
  flash_range_program(CONFIG_BLOCK_START_WRITE, buffer, CONFIG_BLOCK_SIZE);
  
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