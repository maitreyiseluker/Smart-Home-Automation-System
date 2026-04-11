// ESP32 Smart Room Controller

#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASS";

WebServer server(80);

//Pin mapping (according to wiring done in Fritzing ckt)
const int fan_pins[]  = {16, 17, 18, 19}; 
const int bulb_pins[] = {13, 12, 14};     
const int tube_pins[] = {27, 26, 25, 33}; 
const int status_pin  = 2; //Onboard LED 

int fancurrentspeed = 0;
bool bulbstates[3]  = {0, 0, 0};
bool tubestates[4]  = {0, 0, 0, 0};

//Prints error message and time since boot
void log_event(String level, String msg) {
  Serial.printf("[%s] %lu ms: %s\n", level.c_str(), millis(), msg.c_str());
}

//fan control
void set_fanspeed(int speed) {
  if (speed < 0 || speed > 4) {
    log_event("ERROR", "Invalid Fan Speed: " + String(speed));
    return;
  }

  for (int i = 0; i < 4; i++) digitalWrite(fan_pins[i], LOW);
  
  if (speed > 0) {
    digitalWrite(fan_pins[speed - 1], HIGH);
  }
  
  fancurrentspeed = speed;
  log_event("INFO", "Fan Speed Updated to " + String(speed));
}

//bulb/tubelight control
bool update_lights(String type, int id, bool state) {
  int idx = id - 1; // 1-based to 0-based index
  
  if (type == "BULB") {
    if (idx < 0 || idx >= 3) return false;
    bulbstates[idx] = state;
    digitalWrite(bulb_pins[idx], state ? HIGH : LOW);
  } 
  else if (type == "TUBE") {
    if (idx < 0 || idx >= 4) return false;
    tubestates[idx] = state;
    digitalWrite(tube_pins[idx], state ? HIGH : LOW);
  }
  
  log_event("INFO", type + " " + String(id) + (state ? " ON" : " OFF"));
  return true;
}

//Reports current fanspeed and active lights
String getJsonStatus() {
  String s = "{";
  s += "\"fan\":" + String(fancurrentspeed);
  s += ",\"bulbs\":\""; for(int i=0; i<3; i++) s += String(bulbstates[i]);
  s += "\",\"tubes\":\""; for(int i=0; i<4; i++) s += String(tubestates[i]);
  s += "\"}";
  return s;
}

//Web Server handling
void control_handler() {
  if (!server.hasArg("cmd")) {
    server.send(400, "text/plain", "FAIL: Missing 'cmd' argument");
    return;
  }

  String cmd = server.arg("cmd");
  cmd.toUpperCase();
  cmd.trim();

  bool valid_action = false;

  if (cmd == "FAN OFF") {
    set_fanspeed(0);
    valid_action = true;
  } 
  else if (cmd.startsWith("FAN ")) {
    int val = cmd.substring(4).toInt();
    if (val >= 0 && val <= 4) {
      set_fanspeed(val);
      valid_action = true;
    }
  } 
  else if (cmd.startsWith("BULB") || cmd.startsWith("TUBE")) {
    String type = cmd.startsWith("BULB") ? "BULB" : "TUBE";
    int id = cmd.substring(4, 5).toInt();
    bool state = cmd.endsWith("ON");
    valid_action = update_lights(type, id, state);
  }

  if (valid_action) {
    server.send(200, "application/json", getJsonStatus());
  } else {
    log_event("WARNING", "Invalid Command Received: " + cmd);
    server.send(400, "text/plain", "FAIL: Invalid Command Syntax or Bounds");
  }
}

void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i < 4; i++) pinMode(fan_pins[i], OUTPUT);
  for (int i = 0; i < 3; i++) pinMode(bulb_pins[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(tube_pins[i], OUTPUT);
  pinMode(status_pin , OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  log_event("SYSTEM", "Booting. Connecting to WiFi...");

  server.on("/control", HTTP_GET, control_handler);
  server.begin();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
  }

  static unsigned long lastBlink = 0;
  if (millis() - lastBlink > 1000) {
    digitalWrite(status_pin , !digitalRead(status_pin ));
    lastBlink = millis();
  }
}
