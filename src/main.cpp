#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "hal.h"
#include "controllers.h"
#include "command_parser.h"

const char* ssid     = "SSID";
const char* password = "Password";

//pin definitions 
const int fan_pins[]  = {16, 17, 18, 19};
const int bulb_pins[] = {13, 12, 14};
const int tube_pins[] = {27, 26, 25, 33};
const int status_pin  = 2;

fan_controller   fan_ctrl(fan_pins);
light_controller bulb_ctrl(bulb_pins, 3, "BULB");
light_controller tube_ctrl(tube_pins, 4, "TUBE");
command_parser   parser;
WebServer       server(80);

void log_event(const char* level, const String& msg) {
    Serial.printf("[%s] %lu ms: %s\n", level, millis(), msg.c_str());
}

String buildJsonStatus(){
    String s = "{";
    s += "\"fan\":" + String(fan_ctrl.getSpeed());

    s += ",\"bulbs\":\"";
    for (int i = 1; i <= bulb_ctrl.count(); i++)
        s += String(bulb_ctrl.getState(i));

    s += "\",\"tubes\":\"";
    for (int i = 1; i <= tube_ctrl.count(); i++)
        s += String(tube_ctrl.getState(i));

    s += "\"}";
    return s;
}

void control_handler(){
    if (!server.hasArg("cmd")) {
        server.send(400, "text/plain", "FAIL: Missing 'cmd'");
        return;
    }

    command cmd = parser.parse(server.arg("cmd"));
    bool ok = false;

    switch (cmd.type){

        case CommandType::FAN_SET:
            ok = fan_ctrl.setSpeed(cmd.fanSpeed);
            break;

        case CommandType::LIGHT_SET:{
            light_controller& lc =
                (cmd.lightType == LightType::BULB) ? bulb_ctrl : tube_ctrl;
            ok = lc.setState(cmd.lightId, cmd.lightOn);
            break;
        }

        default:
            log_event("WARN", "Invalid command");
            break;
    }

    if (ok)server.send(200, "application/json", buildJsonStatus());
    else server.send(400, "text/plain", "FAIL");
}

void setup(){
    Serial.begin(115200);

    fan_ctrl.init();
    bulb_ctrl.init();
    tube_ctrl.init();
    hal.initPin(status_pin);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    server.on("/control", HTTP_GET, control_handler);
    server.begin();
}

void loop(){
    if (WiFi.status() == WL_CONNECTED) server.handleClient();
    else WiFi.reconnect();

    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 1000){
        hal.togglePin(status_pin);
        lastBlink = millis();
    }
}