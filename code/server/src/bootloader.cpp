#include <Arduino.h>
#include "lib/bootconfig.h"
#include <thread>
#include <functional>
#include <thread>
// #include "flight_controller/flight_controller.cpp"
// #include "flight_controller/pid.cpp" 
#include "connection/connector.h"
#include "include/threading/threading.h"

class bootloader
{
private:



public:
    bootloader(/* args */) {};
    ~bootloader() {};

    void add_thread(std::function<void(void)> function) {
        std::thread thr(function);
        thr.detach();
    };


    
    void boot() {
        Serial.println("DRONE OS");
        Serial.println(BOOTLOADER_VERSION);
        Serial.println(BOOTLOADER_ID);
        Serial.println(BOOTLOADER_MANUFACTURER);

        add_thread(connection);
        
        // example multithreading code
        // the function has to have no arguments and an int as return type
        // add_thread(thread_function)
    };

};

void setup() {
    bootloader booter = bootloader();
    booter.boot();
};

void loop() {

};
