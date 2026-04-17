//#include "flight_controller/flight_controller.cpp"
#include <Arduino.h>
#include <iostream>
#include <esp_sleep.h>
#include <thread>
#include <vector>
#include <functional>
#include <type_traits>
#include "lib/bootconfig.h"
#include "connection/connector.h"
#include "flight_controller/flight_controller.cpp"
#include "flight_controller/pid.cpp"                                                                                   

class bootloader
{
private:



public:
    bootloader(/* args */);
    ~bootloader();

    void add_thread(std::function<int(void)> function) {
        std::thread thr(function);
        thr.detach();
    };


    
    void boot() {
        Serial.println("DRONE OS");
        Serial.println(BOOTLOADER_VERSION);
        Serial.println(BOOTLOADER_ID);
        Serial.println(BOOTLOADER_MANUFACTURER);

        add_thread([]() -> int {
            // example thread function
            while (true) {
                connection();
            }
            return 0;
        });
        
        // example multithreading code
        // the function has to have no arguments and an int as return type
        // add_thread(thread_function)
    };

};
bootloader::bootloader()
{
}

bootloader::~bootloader()
{
}
