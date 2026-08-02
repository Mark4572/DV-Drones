
#include "math.h"

typedef struct Imu_sens {
    Vector3 linear_acceleration;
    Vector3 angular_acceleration;
} Imu_sens;

typedef struct Ps_sens {
    float pressure;
} Ps_sens;

typedef struct Mag_sens {
    Vector3 magnetometer;
} Mag_sens;

typedef struct Sensors {
    Imu_sens imu;
    Ps_sens ps;
    Mag_sens mag;
} Sensors;

