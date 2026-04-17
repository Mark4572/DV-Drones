#include "pid.cpp"

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

        void step(float delta_time) {
            
        };
};

