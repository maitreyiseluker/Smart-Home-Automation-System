#include "hal.h"

HAL hal;

void HAL::init_pin(int pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void HAL::set_relay(int pin, bool state) {
    digitalWrite(pin, state ? HIGH : LOW);
}

bool HAL::read_pin(int pin) {
    return digitalRead(pin) == HIGH;
}

void HAL::toggle_pin(int pin) {
    digitalWrite(pin, !digitalRead(pin));
}
