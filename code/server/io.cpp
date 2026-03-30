#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "ioconfig.h"

WebServer server(80);
void setup();

void blinkError(int times) {
    for (int i = 0; i < times; i++) {
        digitalWrite(ERROR_LED, HIGH);
        delay(100);
        digitalWrite(ERROR_LED, LOW);
        delay(100);
    }
}

void handleCommand() {
    digitalWrite(CMD_LED, HIGH);
    String parts = server.hasArg("part") ? server.arg("part") : "none";
    server.send(200, "application/json", "{\"status\":\"received\"}");
    digitalWrite(CMD_LED, LOW);
}

void handleStatus() {
    digitalWrite(CMD_LED, HIGH);

    int s_wifi   = digitalRead(WIFI_LED);
    int s_server = digitalRead(SERVER_LED);
    int s_cmd    = digitalRead(CMD_LED);
    int s_error  = digitalRead(ERROR_LED);

    
    float rawV = analogRead(BATTERY_PIN);
    float voltage = (rawV / 4095.0) * 3.3 * 2.0;
    
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t uptime = millis() / 1000;
    int rssi = WiFi.RSSI();

    
    String json = "{";
    
    // Hardware & Network
    json += "\"device\": {";
    json += "\"chip_model\":\"" + String(ESP.getChipModel()) + "\",";
    json += "\"chip_revision\":" + String(ESP.getChipRevision()) + ",";
    json += "\"cpu_freq_mhz\":" + String(ESP.getCpuFreqMHz()) + ",";
    json += "\"sdk_version\":\"" + String(ESP.getSdkVersion()) + "\",";
    json += "\"heap_free_bytes\":" + String(freeHeap) + ",";
    json += "\"uptime_seconds\":" + String(uptime) + ",";
    json += "\"ip_address\":\"" + WiFi.localIP().toString() + "\",";
    json += "\"wifi_rssi\":" + String(rssi);
    json += "},";

    // GPIO States
    json += "\"gpio_outputs\": {";
    json += "\"wifi_led_pin_" + String(WIFI_LED) + "\":" + String(s_wifi) + ",";
    json += "\"server_led_pin_" + String(SERVER_LED) + "\":" + String(s_server) + ",";
    json += "\"cmd_led_pin_" + String(CMD_LED) + "\":" + String(s_cmd) + ",";
    json += "\"error_led_pin_" + String(ERROR_LED) + "\":" + String(s_error) + ",";
    json += "\"battery_raw\":" + String(rawV) + ",";
    json += "\"battery_voltage\":" + String(voltage, 2);
    json += "}";

    json += "}";

    server.send(200, "application/json", json);
    
    digitalWrite(CMD_LED, LOW);
}


void initSystem() {
    pinMode(WIFI_LED, OUTPUT);
    pinMode(SERVER_LED, OUTPUT);
    pinMode(CMD_LED, OUTPUT);
    pinMode(ERROR_LED, OUTPUT);
    analogReadResolution(12);

    WiFi.begin(IOCONFIG_SSID, IOCONFIG_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        delay(5500);
        digitalWrite(WIFI_LED, HIGH);
        server.on("/cmd", handleCommand);
        server.on("/status", handleStatus);
        server.on("/", []() {
            server.send(200, "text/plain", "Drone API");
        });
        server.begin();
        digitalWrite(SERVER_LED, HIGH);
    } else {
        blinkError(5);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("boot successful.");
    initSystem();
}

void loop() {
    server.handleClient();
}