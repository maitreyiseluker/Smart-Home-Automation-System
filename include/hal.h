#pragma once
#include <Arduino.h>

class HAL {
public:
    void init_pin(int pin);
    void set_relay(int pin, bool state);
    bool read_pin(int pin);
    void toggle_pin(int pin);
};

extern HAL hal;
