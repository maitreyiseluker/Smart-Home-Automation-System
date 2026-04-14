#pragma once
#include <Arduino.h>

class fan_controller {
public:
    static const int NUM_SPEEDS = 4;

    fan_controller(const int pins[NUM_SPEEDS]);
    void init();
    bool set_speed(int speed);
    int get_speed() const;

private:
    int _pins[NUM_SPEEDS];
    int current_speed = 0;
};

class light_controller {
public:
    light_controller(const int* pins, int count, const char* label);
    ~light_controller();

    void init();
    bool set_state(int id, bool on);
    bool get_state(int id) const;

    int count() const;
    const char* label() const;

private:
    const int* _pins;
    int _count;
    const char* _label;
    bool* _states;
};
