#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "ioconfig.h"

static constexpr uint8_t ADXL345_ADDR_DEFAULT = 0x53;
static constexpr uint8_t ADXL345_ADDR_ALT = 0x1D;
static constexpr uint8_t ADXL345_REG_DEVID = 0x00;
static constexpr uint8_t ADXL345_DEVICE_ID = 0xE5;
static constexpr uint8_t ADXL345_REG_POWER_CTL = 0x2D;
static constexpr uint8_t ADXL345_REG_DATA_FORMAT = 0x31;
static constexpr uint8_t ADXL345_REG_BW_RATE = 0x2C;
static constexpr uint8_t ADXL345_REG_DATAX0 = 0x32;

struct Config {
    const char* ssid = SSID;
    const char* password = PASSWORD;
    const int port = PORT;
    const int status_led = STATUS_LED;
    const int battery_pin = BATTERY_PIN;
};

Config cfg;

void scanI2CBus() {
    Serial.println("\n=== I2C Bus Scan ===");
    byte count = 0;
    for (byte i = 8; i < 120; i++) {
        Wire.beginTransmission(i);
        if (Wire.endTransmission() == 0) {
            Serial.printf("Device found at I2C address 0x%02X\n", i);
            count++;
        }
    }
    Serial.printf("Total devices found: %d\n", count);
    if (count == 0) {
        Serial.println("WARN: No I2C devices detected!");
        Serial.println("");
    }
    Serial.println("====================\n");
}

class SensorSubsystem {
private:
    float currentLat = 0.0;
    float currentLng = 0.0;
    float currentHeading = 0.0;
    float currentTiltX = 0.0;
    float currentTiltY = 0.0;
    float currentTiltZ = 0.0;
    bool adxlReady = false;
    uint8_t adxlAddr = ADXL345_ADDR_DEFAULT;

    bool writeRegister(uint8_t reg, uint8_t value) {
        Wire.beginTransmission(adxlAddr);
        Wire.write(reg);
        Wire.write(value);
        return Wire.endTransmission() == 0;
    }

    bool readRegister(uint8_t reg, uint8_t& value) {
        Wire.beginTransmission(adxlAddr);
        Wire.write(reg);
        if (Wire.endTransmission(false) != 0) return false;
        if (Wire.requestFrom(adxlAddr, (uint8_t)1) != 1) return false;
        value = Wire.read();
        return true;
    }

    bool detectAdxl(uint8_t addr) {
        adxlAddr = addr;
        uint8_t deviceId = 0;
        if (!readRegister(ADXL345_REG_DEVID, deviceId)) return false;
        return deviceId == ADXL345_DEVICE_ID;
    }

    bool initAdxl() {
        if (!detectAdxl(ADXL345_ADDR_DEFAULT) && !detectAdxl(ADXL345_ADDR_ALT)) {
            Serial.println("ADXL345 not found at primary or alternate address");
            return false;
        }

        if (!writeRegister(ADXL345_REG_DATA_FORMAT, 0x0B)) {
            return false;
        }
        if (!writeRegister(ADXL345_REG_BW_RATE, 0x0A)) {
            return false;
        }
        if (!writeRegister(ADXL345_REG_POWER_CTL, 0x08)) {
            return false;
        }

        Serial.printf("ADXL345 initialized at I2C address 0x%02X\n", adxlAddr);
        return true;
    }

    void readAdxl() {
        Wire.beginTransmission(adxlAddr);
        Wire.write(ADXL345_REG_DATAX0);
        if (Wire.endTransmission(false) != 0) return;

        if (Wire.requestFrom(adxlAddr, (uint8_t)6) != 6) return;

        int16_t rawX = Wire.read() | (Wire.read() << 8);
        int16_t rawY = Wire.read() | (Wire.read() << 8);
        int16_t rawZ = Wire.read() | (Wire.read() << 8);

        const float gPerLSB = 0.0039f;
        float ax = rawX * gPerLSB;
        float ay = rawY * gPerLSB;
        float az = rawZ * gPerLSB;

        currentTiltX = atan2(ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
        currentTiltY = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / PI;
        currentTiltZ = atan2(az, sqrt(ax * ax + ay * ay)) * 180.0 / PI;
    }

public:
    void init() {
        pinMode(cfg.battery_pin, INPUT);
        Wire.begin(21, 22);
        Wire.setClock(400000);
        
        delay(100);
        scanI2CBus();
        
        adxlReady = initAdxl();
        if (!adxlReady) {
            Serial.println("ADXL345 init failed");
        } else {
            Serial.println("ADXL345 initialized");
        }
    }

    void update() {
        if (adxlReady) {
            readAdxl();
        }
    }

    float getBatteryVoltage() {
        float rawV = analogRead(cfg.battery_pin);
        return (rawV / 4095.0) * 3.3 * 2.0;
    }

    float getTiltX() const { return currentTiltX; }
    float getTiltY() const { return currentTiltY; }
    float getTiltZ() const { return currentTiltZ; }

    // Getters for navigation
    float getLat() { return currentLat; }
    float getLng() { return currentLng; }
    float getHeading() { return currentHeading; }
};

class FlightController {
private:
    int motorPins[4] = {12, 13, 14, 15}; 
    
public:
    void init() {
        for(int i=0; i<4; i++) {

        }
    }

    void stabilize() {

    }

    void setTargetVector(float speed, float heading, float altitude) {
        // Adjusts the PID targets based on where we want to go
    }
};


class NavigationSystem {
private:
    SensorSubsystem* sensors;
    FlightController* flight;
    
    float targetLat = 0.0;
    float targetLng = 0.0;
    bool isNavigating = false;

public:
    NavigationSystem(SensorSubsystem* s, FlightController* f) : sensors(s), flight(f) {}

    void setDestination(float lat, float lng) {
        targetLat = lat;
        targetLng = lng;
        isNavigating = true;
    }

    bool isNavigatingPublic() const { return isNavigating; }
    float getTargetLat() const { return targetLat; }
    float getTargetLng() const { return targetLng; }

    void updateRoute() {
        if (!isNavigating) return;
        float distance = calculateDistance(sensors->getLat(), sensors->getLng(), targetLat, targetLng);
        float bearing = calculateBearing(sensors->getLat(), sensors->getLng(), targetLat, targetLng);

        
        if (obstacleDetected()) {
            evadeObstacle();
            return;
        }

        
        if (distance > 2.0) {
            flight->setTargetVector(5.0, bearing, 10.0); //values
        } else {
            isNavigating = false;
            
        }
    }

    float calculateETA() {
        return 0.0; 
    }

private:
    bool obstacleDetected() { return false;}
    void evadeObstacle() {}
    float calculateDistance(float lat1, float lon1, float lat2, float lon2) { return 0.0; /* Haversine math */ }
    float calculateBearing(float lat1, float lon1, float lat2, float lon2) { return 0.0; /* Bearing math */ }
};

extern FlightController flight;

class GroundStation;
static GroundStation* gsInstance = nullptr;
void wsEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

class GroundStation {
private:
    WebServer server;
    WebSocketsServer webSocket;
    SensorSubsystem* sensors;
    NavigationSystem* nav;

public:
    GroundStation(int port, SensorSubsystem* s, NavigationSystem* n) : server(port), webSocket(81), sensors(s), nav(n) {}

    void handleWsEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
        if (type == WStype_DISCONNECTED) {
            Serial.printf("WS[%u] Disconnected\n", num);
            return;
        }
        if (type == WStype_CONNECTED) {
            Serial.printf("WS[%u] Connected\n", num);
            return;
        }
        if (type == WStype_TEXT) {
            DynamicJsonDocument doc(256);
            DeserializationError err = deserializeJson(doc, payload, length);
            if (err) {
                Serial.println("Failed to parse WS JSON");
                webSocket.sendTXT(num, "{\"error\":\"bad_json\"}");
                return;
            }

            const char* typeStr = doc["type"] | "";
            if (strcmp(typeStr, "set_target") == 0) {
                float lat = doc["lat"] | 0.0f;
                float lng = doc["lng"] | 0.0f;
                nav->setDestination(lat, lng);
                webSocket.sendTXT(num, "{\"result\":\"ok\"}");
                Serial.printf("WS set_target %f,%f\n", lat, lng);
            } else if (strcmp(typeStr, "ping") == 0) {
                webSocket.sendTXT(num, "{\"result\":\"pong\"}");
            } else if (strcmp(typeStr, "motor") == 0) {
                int idx = doc["index"] | -1;
                int val = doc["value"] | 0;
                Serial.printf("WS motor idx=%d val=%d\n", idx, val);
                webSocket.sendTXT(num, "{\"result\":\"ok\"}");
            } else if (strcmp(typeStr, "arm") == 0) {
                Serial.println("WS arm");
                webSocket.sendTXT(num, "{\"result\":\"armed\"}");
            } else if (strcmp(typeStr, "disarm") == 0) {
                Serial.println("WS disarm");
                webSocket.sendTXT(num, "{\"result\":\"disarmed\"}");
            } else {
                webSocket.sendTXT(num, "{\"error\":\"unknown_command\"}");
            }
        }
    }

    void init() {
        Serial.println("Connecting to WiFi...");
        WiFi.mode(WIFI_STA);
        WiFi.begin(cfg.ssid, cfg.password);
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 40) { 
            delay(500);
            Serial.print(".");
            attempts++;
        }
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nWiFi connected!");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("\nFailed to connect to WiFi");
        }

        server.on("/set_target", HTTP_POST, [this]() {
            float lat = server.arg("lat").toFloat();
            float lng = server.arg("lng").toFloat();
            nav->setDestination(lat, lng);
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "text/plain", "Target acquired");
        });

        server.on("/telemetry", [this]() {
            DynamicJsonDocument doc(192);
            doc["batt"] = sensors->getBatteryVoltage();
            doc["lat"] = sensors->getLat();
            doc["lng"] = sensors->getLng();
            doc["tilt_x"] = sensors->getTiltX();
            doc["tilt_y"] = sensors->getTiltY();
            doc["tilt_z"] = sensors->getTiltZ();
            String out;
            serializeJson(doc, out);
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "application/json", out);
        });

        server.on("/status", [this]() {
            DynamicJsonDocument doc(320);
            doc["wifi_connected"] = (WiFi.status() == WL_CONNECTED);
            doc["local_ip"] = WiFi.localIP().toString();
            doc["uptime_seconds"] = millis() / 1000;
            doc["free_heap"] = ESP.getFreeHeap();
            doc["battery_voltage"] = sensors->getBatteryVoltage();
            doc["navigation_active"] = nav->isNavigatingPublic();
            doc["target_lat"] = nav->getTargetLat();
            doc["target_lng"] = nav->getTargetLng();
            doc["current_lat"] = sensors->getLat();
            doc["current_lng"] = sensors->getLng();
            doc["current_heading"] = sensors->getHeading();
            doc["tilt_x"] = sensors->getTiltX();
            doc["tilt_y"] = sensors->getTiltY();
            doc["tilt_z"] = sensors->getTiltZ();
            String out;
            serializeJson(doc, out);
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.send(200, "application/json", out);
        });

        server.begin();
        gsInstance = this;
        webSocket.begin();
        webSocket.onEvent(wsEvent);
    }

    void handleClients() {
        server.handleClient();
        webSocket.loop();
    }

    void broadcastTelemetry() {

        DynamicJsonDocument doc(192);
        doc["batt"] = sensors->getBatteryVoltage();
        doc["lat"] = sensors->getLat();
        doc["lng"] = sensors->getLng();
        doc["tilt_x"] = sensors->getTiltX();
        doc["tilt_y"] = sensors->getTiltY();
        doc["tilt_z"] = sensors->getTiltZ();
        String out;
        serializeJson(doc, out);
        webSocket.broadcastTXT(out);
    }
};

void wsEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (gsInstance) {
        gsInstance->handleWsEvent(num, type, payload, length);
    }
}


SensorSubsystem sensors;
FlightController flight;
NavigationSystem nav(&sensors, &flight);
GroundStation groundStation(cfg.port, &sensors, &nav);


void setup() {
    Serial.begin(115200);
    sensors.init();
    flight.init();
    groundStation.init();
}

void loop() {
    sensors.update();
    nav.updateRoute();
    flight.stabilize();
    groundStation.handleClients();
    
    
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 500) {
        groundStation.broadcastTelemetry();
        lastUpdate = millis();
    }
}
