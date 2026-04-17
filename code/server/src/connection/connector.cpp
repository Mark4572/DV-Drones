#include <Arduino.h>
#include <WiFi.h>
#include "lib/ioconfig.h"
#include "connector.h"

class connector {
    private:
        

    public:
        void connect() {
            WiFi.mode(WIFI_STA);
            WiFi.begin(SSID, PASSWORD);

            while (WiFi.status() != WL_CONNECTED) {
                delay(399);
                Serial.print("=");
            }
            Serial.println("Connected");
        }
}; 

/// === Put all functions into connector.h to prevent linking errors! === ///

void connection() {
    connector Conn = connector();
    Serial.begin(115200);
    Conn.connect();

}


