#include "controllers.h"
#include "hal.h"

fan_controller::fan_controller(const int pins[NUM_SPEEDS]) {
    for (int i = 0; i < NUM_SPEEDS; i++) _pins[i] = pins[i];
}

void fan_controller::init() {
    for (int i = 0; i < NUM_SPEEDS; i++)
        hal.init_pin(_pins[i]);
}

bool fan_controller::set_speed(int speed) {
    if (speed < 0 || speed > NUM_SPEEDS) return false;

    for (int i = 0; i < NUM_SPEEDS; i++)
        hal.set_relay(_pins[i], false);

    if (speed > 0)
        hal.set_relay(_pins[speed - 1], true);

    current_speed = speed;
    return true;
}

int fan_controller::get_speed() const {
    return current_speed;
}


light_controller::light_controller(const int* pins, int count, const char* label)
    : _pins(pins), _count(count), _label(label) {
    _states = new bool[count]();
}

light_controller::~light_controller() {
    delete[] _states;
}

void light_controller::init() {
    for (int i = 0; i < _count; i++)
        hal.init_pin(_pins[i]);
}

bool light_controller::set_state(int id, bool on) {
    int idx = id - 1;
    if (idx < 0 || idx >= _count) return false;

    _states[idx] = on;
    hal.set_relay(_pins[idx], on);
    return true;
}

bool light_controller::get_state(int id) const {
    int idx = id - 1;
    if (idx < 0 || idx >= _count) return false;
    return _states[idx];
}

int light_controller::count() const {
    return _count;
}

const char* light_controller::label() const {
    return _label;
}
