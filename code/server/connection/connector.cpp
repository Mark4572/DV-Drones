
#include <Arduino.h>
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting...");
  WiFi.begin("dv_gate", "dv_drones");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print("‖");
  }
  Serial.println("Connected!");
}
