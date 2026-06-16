#include <Arduino.h>
#include "esp32-hal.h"
#include "lib/bootconfig.h"
#include "flight_controller/flight_controller.hpp"
#include "flight_controller/pid.hpp" 
#include "connection/connector.h"
#include "include/threading/threading.h"

flight_controller fc = flight_controller(pid_controller(1.0, 0.0, 0.0), pid_controller(1.0, 0.0, 0.0), pid_controller(1.0, 0.0, 0.0));

class bootloader
{
private:



public:
    bootloader(/* args */) {};
    ~bootloader() {};

    thread_controller thread_handler;
    
    void boot() {
        Serial.println("DRONE OS");
        Serial.println(BOOTLOADER_VERSION);
        Serial.println(BOOTLOADER_ID);
        Serial.println(BOOTLOADER_MANUFACTURER);
        
        // example multithreading code
        // the function has to have no arguments and an int as return type
        // add_thread(thread_function) 
        // arguments: TaskFunction_t pvTaskCode, const char *constpcName, const uint32_t usStackDepth, void *constpvParameters, UBaseType_t uxPriority

        thread_controller thread_handler = thread_controller();

    };

    void step(double delta_time) {
        fc.step(delta_time);
    };

};


bootloader booter = bootloader();

void setup() {

    Serial.begin(115200);
    delay(1000);
    booter.boot();

}

unsigned long last_time = 0;

void loop() {
    // dt calculation
    unsigned long current_time = millis();
    double dt = (current_time - last_time) / 1000.0;
    last_time = current_time;

    booter.step(dt);

    delay(random(20, 50));

    unsigned long step_time = millis() - current_time;
    Serial.println("Delta time:" + String(dt, 6) + " s");
    Serial.println("Step time: " + String(step_time, 6) + " ms");
    delay(100-step_time);
}
