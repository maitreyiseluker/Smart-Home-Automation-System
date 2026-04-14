#pragma once
#include <Arduino.h>

enum class CommandType {
    FAN_SET,
    LIGHT_SET,
    UNKNOWN
};

enum class LightType {
    BULB,
    TUBE,
    NONE
};

struct command {
    CommandType type = CommandType::UNKNOWN;

    int fanSpeed = 0;

    LightType lightType = LightType::NONE;
    int lightId = 0;
    bool lightOn = false;
};

class command_parser {
public:
    command parse(String raw);

private:
    command parse_fan(const String& raw);
    command parse_light(const String& raw);
};
