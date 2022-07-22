// Debounce that's based on eager_pk and speedypotato's minimum hold
// Sends report immediately when switch is triggered and ignores extra inputs for n amount of time
// Faster, but may have random triggers due to electrical noise

#include "debounce.h"

//all of this here is just placeholder code
//there is a compilation error that it cannot read swGPIO or the other standard libraries from main.c
bool is_pressed;

bool eager_debounce() {
    for (uint8_t i = 0; i < swGPIOsize; i++){
        if(!gpio_get(swGPIO[i]) && time_us_64() - swTimestamp[i] <= swDebounceTime_us) {
            is_pressed = true;
        } else {
            is_pressed = false;
        }
    }
    return is_pressed;
}