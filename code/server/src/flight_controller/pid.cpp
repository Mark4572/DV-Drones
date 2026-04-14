class pid {

    private:
        float p;
        float i;
        float d;

        float integral;
        float prev_error;

        float calculate_p(float error, float delta_time) {
            return this->p * error;
        };

        float calculate_i(float delta_time) {
            return this->i * this->integral * delta_time;
        };

        float calculate_d(float error, float delta_time) {
            return ((error - this->prev_error) / delta_time) * this->d;
        };

    public:
        pid(float p, float i, float d) {
            this->p = p;
            this->i = i;
            this->d = d;
        };

        ~pid() {};

        float run_pid(float setpoint, float value, float delta_time) { // delta_time in seconds
            float error = setpoint - value;
            this->integral += error * delta_time;


            float output = 0;
            output += calculate_p(error, delta_time);
            output += calculate_i(delta_time);
            output += calculate_d(error, delta_time);

            this->prev_error = error;
            return output;
        };

        void set_p(float p) {
            this->p = p;
        };

        void set_i(float i) {
            this->i = i;
        };

        void set_d(float d) {
            this->d = d;
        };

};