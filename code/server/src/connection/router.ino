#include <WiFi.h>
#include "esp_wifi.h"
#include "lwip/lwip_napt.h"
#include "dhcpserver/dhcpserver.h"
#include <Preferences.h>

Preferences prefs;

#define LED_PIN 2
bool wifiConnected = false;
bool naptEnabled = false;

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    prefs.begin("esp32_nat", true); 


    String ssid = prefs.getString("sta_ssid", "TP-Link_93F0");
    String pass = prefs.getString("sta_pwd", "31328845");
    String ap_ssid = prefs.getString("ap_ssid", "ESP32WIFI");
    String ap_pass = prefs.getString("ap_pwd", "ESPWIFI@Home");
    prefs.end();

    WiFi.mode(WIFI_AP_STA);

    IPAddress apIP(192, 168, 4, 1);
    IPAddress apGateway(192, 168, 4, 1);
    IPAddress apSubnet(255, 255, 255, 0);
    WiFi.softAPConfig(apIP, apGateway, apSubnet);
    WiFi.softAP(ap_ssid.c_str(), ap_pass.c_str());

    Serial.print("Access Point started: ");
    Serial.println(ap_ssid);

    WiFi.begin(ssid.c_str(), pass.c_str());
    Serial.print("Connecting to: ");
    Serial.println(ssid);
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        wifiConnected = false;
        naptEnabled = false;
        digitalWrite(LED_PIN, HIGH);
        delay(150);
        digitalWrite(LED_PIN, LOW);
        delay(150);
        return;
    }


    if (!wifiConnected) {
        wifiConnected = true;
        Serial.println("\nConnected to Home WiFi!");
        Serial.print("Internal IP: ");
        Serial.println(WiFi.localIP());
    }

 
    if (!naptEnabled) {

        IPAddress local_IP(192, 168, 4, 1);
        IPAddress gateway(192, 168, 4, 1);
        IPAddress subnet(255, 255, 255, 0);
        WiFi.softAPConfig(local_IP, gateway, subnet);
        WiFi.softAP("ESPWIFI", "ESPWIFI@Home");
        naptEnabled = true;
        Serial.println("NAPT Router logic is now ACTIVE");
    }


    digitalWrite(LED_PIN, HIGH);
    delay(100);
}