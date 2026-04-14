#include "command_parser.h"

command command_parser::parse(String raw) {
    raw.trim();
    raw.toUpperCase();

    command cmd;

    if (raw == "FAN OFF") {
        cmd.type = CommandType::FAN_SET;
        cmd.fanSpeed = 0;
    }
    else if (raw.startsWith("FAN ")) {
        cmd = parse_fan(raw);
    }
    else if (raw.startsWith("BULB") || raw.startsWith("TUBE")) {
        cmd = parse_light(raw);
    }

    return cmd;
}

command command_parser::parse_fan(const String& raw) {
    command cmd;
    int speed = raw.substring(4).toInt();

    if (speed >= 0 && speed <= 4) {
        cmd.type = CommandType::FAN_SET;
        cmd.fanSpeed = speed;
    }

    return cmd;
}

command command_parser::parse_light(const String& raw) {
    command cmd;

    bool isBulb = raw.startsWith("BULB");
    int id = raw.substring(4, 5).toInt();
    bool stateOn = raw.endsWith("ON");
    bool stateOff = raw.endsWith("OFF");

    if (id > 0 && (stateOn || stateOff)) {
        cmd.type = CommandType::LIGHT_SET;
        cmd.lightType = isBulb ? LightType::BULB : LightType::TUBE;
        cmd.lightId = id;
        cmd.lightOn = stateOn;
    }

    return cmd;
}
