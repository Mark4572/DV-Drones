#include "threading.h"

thread_controller::thread_controller() : thread_amount(0) {
    for (int i = 0; i < MAX_THREADS; i++) {
        threads[i] = NULL;
    }
}

thread_controller::~thread_controller() {}

// Notice: No default assignment values (=) are allowed here
TaskHandle_t *const thread_controller::add_thread(TaskFunction_t pvTaskCode, const char *const pcName, const uint32_t usStackDepth, void *const pvParameters, UBaseType_t uxPriority) {
    if (this->thread_amount >= MAX_THREADS) {
        return nullptr; 
    }

    TaskHandle_t local_task_handle = NULL;
    BaseType_t result = xTaskCreate(pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, &local_task_handle);

    if (result != pdPASS) {
        return nullptr;
    }

    this->threads[this->thread_amount] = local_task_handle;
    this->thread_amount += 1;

    return &(this->threads[this->thread_amount - 1]);
}