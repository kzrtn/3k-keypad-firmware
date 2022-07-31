cmake_minimum_required(VERSION 3.13)

add_executable(3k_keypad
        main.c
        usb_descriptors.c
        EventGenerator.c
        StateMachine.c
        FlashStorage.c)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(3k_keypad PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_sources(3k_keypad PRIVATE main.c)

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(3k_keypad PRIVATE
        pico_multicore
        pico_stdlib
        hardware_pio
        hardware_dma
        hardware_flash
        hardware_sync
        tinyusb_device
        tinyusb_board)

pico_add_extra_outputs(3k_keypad)

# pico_enable_stdio_usb(multicore 1)
# pico_enable_stdio_uart(multicore 0)
