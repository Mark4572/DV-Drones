#include "connection/connector.h"
#include <Arduino.h>
Config cfg;
WebServer server(cfg.port);

void handleInit() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "Init OK");
}

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

    void readBattery(float& voltage) {
        float rawV = analogRead(cfg.battery_pin);
        voltage = (rawV / 4095.0) * 3.3 * 2.0;
    }
};

class Network {
public:
    bool connect(const char* s, const char* p) {
        WiFi.mode(WIFI_STA);
        WiFi.setHostname("DRN-001");
        WiFi.begin(s, p);

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            attempts++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Connected as STA: " + WiFi.localIP().toString());
            return true;
        }

        Serial.println("Station connect failed");
        return false;
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

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
    
    dev.setStatus(LOW);
}

void setup() {
    Serial.begin(115200);
    dev.init(cfg.status_led);

    if (net.connect(cfg.ssid, cfg.password)) {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Connected: " + WiFi.localIP().toString());
        } else {
            Serial.println("Running in AP mode: " + WiFi.softAPIP().toString());
        }
        
        server.on("/", HTTP_GET, []() {
            server.send(200, "text/plain", "Drone server running");
        });
        server.on("/status", HTTP_GET, handleStatus);
        server.on("/init", HTTP_GET, handleInit);
        server.onNotFound([]() {
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(404, "text/plain", "Not Found");
        });
        server.begin();
        
        for(int i=0; i<2; i++) { dev.setStatus(1); delay(100); dev.setStatus(0); delay(100); }
    } else {
        Serial.println("WiFi Failed");
    }
}

void loop() {
    connection();
}

void connection() {
    server.handleClient();
}
