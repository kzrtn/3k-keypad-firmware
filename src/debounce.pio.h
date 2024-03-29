// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// -------- //
// debounce //
// -------- //

#define debounce_wrap_target 2
#define debounce_wrap 14

static const uint16_t debounce_program_instructions[] = {
    0x80a0, //  0: pull   block                      
    0xa027, //  1: mov    x, osr                     
            //     .wrap_target
    0x8080, //  2: pull   noblock                    
    0xa027, //  3: mov    x, osr                     
    0x2020, //  4: wait   0 pin, 0                   
    0xe041, //  5: set    y, 1                       
    0xa0c2, //  6: mov    isr, y                     
    0x8000, //  7: push   noblock                    
    0x00cb, //  8: jmp    pin, 11                    
    0xa041, //  9: mov    y, x                       
    0x0008, // 10: jmp    8                          
    0x0088, // 11: jmp    y--, 8                     
    0xe040, // 12: set    y, 0                       
    0xa0c2, // 13: mov    isr, y                     
    0x8000, // 14: push   noblock                    
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program debounce_program = {
    .instructions = debounce_program_instructions,
    .length = 15,
    .origin = -1,
};

static inline pio_sm_config debounce_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + debounce_wrap_target, offset + debounce_wrap);
    return c;
}

static inline void debounce_program_init(PIO pio, uint sm, uint offset, uint debouncePin) 
{
    pio_sm_config c = debounce_program_get_default_config(offset);
    pio_gpio_init(pio, debouncePin);
    gpio_pull_up(debouncePin);
    sm_config_set_in_pins(&c, debouncePin);
    sm_config_set_jmp_pin (&c, debouncePin);
    pio_sm_set_consecutive_pindirs(pio, sm, debouncePin, 1, false);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}
static inline void debounce_program_set_debounce(PIO pio, uint sm, float debounceDurationMs)
{
    float tickDurationMs = 1000.0f / clock_get_hz(clk_sys);                 // The duration of 1 clock tick expressed in milliseconds
    uint numberOfCounterLoops = debounceDurationMs / (2 * tickDurationMs);  // 1 count loop takes 2 clock ticks
    pio_sm_put(pio, sm, numberOfCounterLoops);
}
static inline bool debounce_program_get_button_pressed(PIO pio, uint sm)
{
    // The static variable keeps its value across multiple function calls
    // This variable keeps track of the button state for all 8 possible state machines
    static uint buttonStates = 0;
    // Get the index of the button state
    uint index = (pio == pio0) ? sm : 4 + sm;
    // Drain the buffer and only keep the last state as this is the most recent one
    while(!pio_sm_is_rx_fifo_empty(pio, sm))
    {
        if(pio_sm_get(pio, sm) != 0)
            buttonStates |= (1 << index);
        else
            buttonStates &= ~(1 << index);
    }
    return ((buttonStates >> index) & 1);
}

#endif

