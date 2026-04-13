#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "ioconfig.h"

struct Config {
    const char* ssid = SSID;
    const char* password = PASSWORD;
    const int port = PORT;
    const int status_led = STATUS_LED;
    const int battery_pin = BATTERY_PIN;
};

Config cfg;
WebServer server(cfg.port);

class DeviceState {
public:
    void init(int pin) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        analogReadResolution(12);
    }

    void setStatus(bool active) {
        digitalWrite(cfg.status_led, active ? HIGH : LOW);
    }
};

class Network {
public:
    bool connect(const char* s, const char* p) {
        WiFi.begin(s, p);
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            attempts++;
        }
        return WiFi.status() == WL_CONNECTED;
    }
};

DeviceState dev;
Network net;

void handleStatus() {
    dev.setStatus(HIGH);

    float rawV = analogRead(cfg.battery_pin);
    float voltage = (rawV / 4095.0) * 3.3 * 2.0;

    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"uptime\":" + String(millis() / 1000) + ",";
    json += "\"batt_v\":" + String(voltage, 2);
    json += "}";

    server.send(200, "application/json", json);
    
    dev.setStatus(LOW);
}

void setup() {
    Serial.begin(115200);
    dev.init(cfg.status_led);

    if (net.connect(cfg.ssid, cfg.password)) {
        Serial.println("Connected: " + WiFi.localIP().toString());
        
        server.on("/status", handleStatus);
        server.onNotFound([]() {
            server.send(404, "text/plain", "Not Found");
        });
        server.on("/init", init);
    server.begin();
        
        server.begin();
        for(int i=0; i<2; i++) { dev.setStatus(1); delay(100); dev.setStatus(0); delay(100); }
    } else {
        Serial.println("WiFi Failed");
    }
}

void loop() {
    server.handleClient();
}
