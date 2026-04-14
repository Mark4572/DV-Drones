#include "pid.cpp"

class flight_controller {

    private:

        pid roll_pid;
        pid yaw_pid;
        pid pitch_pid;

    public:

        flight_controller(pid roll_pid, pid yaw_pid, pid pitch_pid){

            this->roll_pid = roll_pid;
            this->yaw_pid = yaw_pid;
            this->pitch_pid = pitch_pid;
        };
};