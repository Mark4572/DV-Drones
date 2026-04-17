#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "lib/ioconfig.h"

struct Config {
    const char* ssid = SSID;
    const char* password = PASSWORD;
    const int port = PORT;
    const int status_led = STATUS_LED;
    const int battery_pin = BATTERY_PIN;
};

extern Config cfg;
extern WebServer server;

void handleInit();
void handleStatus();
void connection();

#endif // CONNECTOR_H
