// Debounce that's based on eager_pk and speedypotato's minimum hold
// Sends report immediately when switch is triggered and ignores extra inputs for n amount of time
// Faster, but may have random triggers due to electrical noise
bool is_pressed;

//getting error that it's not recognising variables from main.c
//also I get the feeling that using is_pressed boolean here to replace isPressed from main.c is not optimal

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