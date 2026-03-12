#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>


class PID {
public:
    double kp, ki, kd;
    double integral = 0, prevError = 0;

    PID(double p, double i, double d) : kp(p), ki(i), kd(d) {}

    double calculate(double setpoint, double actual, double dt) {
        double error = setpoint - actual;
        integral += error * dt;
        double derivative = (error - prevError) / dt;
        double output = (kp * error) + (ki * integral) + (kd * derivative);
        prevError = error;
        return output;
    }
};


struct Orientation {
    double pitch, roll, yaw;
};

class DroneHardware {
public:
 
    Orientation readSensor() {
        return {0.5, -0.2, 0.1};
    }


    void applyMotorSpeed(int id, double speed) {
     
        speed = std::max(0.0, std::min(100.0, speed));
        std::cout << "Motor " << id << " Speed: " << (int)speed << "%" << std::endl;
    }
};


int main() {
    DroneHardware drone;
    

    PID pitchPID(1.5, 0.05, 0.4);
    PID rollPID(1.5, 0.05, 0.4);
    PID yawPID(2.0, 0.1, 0.1);

  
    double targetPitch = 0.0, targetRoll = 0.0, targetYaw = 0.0;
    double throttle = 60.0; 
    
    double dt = 0.01; 

    std::cout << "Starting Drone Controller Loop..." << std::endl;
    std::cout << "-----------------------------------" << std::endl;


    for (int i = 0; i < 10; ++i) {
        Orientation current = drone.readSensor();

  
        double pOut = pitchPID.calculate(targetPitch, current.pitch, dt);
        double rOut = rollPID.calculate(targetRoll, current.roll, dt);
        double yOut = yawPID.calculate(targetYaw, current.yaw, dt);

        drone.applyMotorSpeed(0, throttle - pOut + rOut - yOut);
        drone.applyMotorSpeed(1, throttle - pOut - rOut + yOut);
        drone.applyMotorSpeed(2, throttle + pOut + rOut + yOut);
        drone.applyMotorSpeed(3, throttle + pOut - rOut - yOut);

        std::cout << "--- Loop " << i << " Complete ---" << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
