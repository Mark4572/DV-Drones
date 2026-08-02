
typedef struct Pid{
    float p;
    float i;
    float d;

    float integral;
    float previous_error;
    float clamp_min;
    float clamp_max;
} Pid;

void pid_init(Pid *pid, float p, float i, float d, float clamp_min, float clamp_max) {
    pid->p = p;
    pid->i = i;
    pid->d = d;
    pid->integral = 0.0f;
    pid->previous_error = 0.0f;
    pid->clamp_min = clamp_min;
    pid->clamp_max = clamp_max;
}

void pid_reset(Pid *pid) {
    pid->integral = 0.0f;
    pid->previous_error = 0.0f;
}

void pid_run(Pid *pid, float setpoint, float measured_value, float dt, float *output) {
    float error = setpoint - measured_value;

    // Proportional term
    float p_term = pid->p * error;

    // Integral term
    pid->integral += error * dt;
    // Clamp the integral term to prevent windup
    if (pid->integral > pid->clamp_max) {
        pid->integral = pid->clamp_max;
    } else if (pid->integral < pid->clamp_min) {
        pid->integral = pid->clamp_min;
    }
    float i_term = pid->i * pid->integral;

    // Derivative term
    float derivative = (error - pid->previous_error) / dt;
    float d_term = pid->d * derivative;

    // Compute the output
    *output = p_term + i_term + d_term;

    // Update previous error
    pid->previous_error = error;
}
