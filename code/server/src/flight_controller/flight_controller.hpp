#pragma once

#include "pid.hpp"
#include <Arduino.h>

class flight_controller {

    private:

        pid_controller roll_pid;
        pid_controller yaw_pid;
        pid_controller pitch_pid;
        // io_controller io_handler;

    public:
        flight_controller(pid_controller roll_pid, pid_controller yaw_pid, pid_controller pitch_pid) {
            this->roll_pid = roll_pid;
            this->yaw_pid = yaw_pid;
            this->pitch_pid = pitch_pid;    
        };

        void step(double delta_time) {
            Serial.println(this->roll_pid.run_pid(0.0f, 0.0f, delta_time));
            Serial.println(this->yaw_pid.run_pid(0.0f, 0.0f, delta_time));
            Serial.println(this->pitch_pid.run_pid(0.0f, 0.0f, delta_time));
        };
};

