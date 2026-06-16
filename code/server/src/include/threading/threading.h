#ifndef THREADING_H
#define THREADING_H

#include "Arduino.h"

#define MAX_THREADS 5 // Adjust as needed

class thread_controller {
private:
    TaskHandle_t threads[MAX_THREADS];
    int thread_amount;

public:
    thread_controller();
    ~thread_controller();

    // Default arguments go HERE in the declaration:
    TaskHandle_t *const add_thread(
        TaskFunction_t pvTaskCode, 
        const char *const pcName = "ESP32_Task", 
        const uint32_t usStackDepth = 2048, 
        void *const pvParameters = nullptr, 
        UBaseType_t uxPriority = 1
    );
};

#endif