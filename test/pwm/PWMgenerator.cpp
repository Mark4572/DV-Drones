#include <Arduino.h>

#define MOTOR_PIN 4
#define PWM_CHANNEL 0
#define PWM_FREQ 50 // 50Hz refresh rate
#define PWM_RES 16  // 16-bit resolution (0 to 65535)


volatile uint32_t current_throttle_us = 1000; 


void update_esc_signal(uint32_t us) {
    // Map microseconds (1000-2000) directly to 16-bit hardware timer steps
    uint32_t duty = (us * 65536) / 20000;

    #if ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcWrite(MOTOR_PIN, duty); 
    #else
        ledcWrite(PWM_CHANNEL, duty); 
    #endif
}


void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("=== TEST ===");

    #if ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcAttach(MOTOR_PIN, PWM_FREQ, PWM_RES);
    #else
        ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
        ledcAttachPin(MOTOR_PIN, PWM_CHANNEL);
    #endif


    Serial.println("Booting ESC...");
    current_throttle_us = 1500;
    update_esc_signal(current_throttle_us);
    delay(10000); 

    Serial.println("Unlocking..");
    
    current_throttle_us = 2000;
    update_esc_signal(current_throttle_us);
    delay(5000);
    Serial.println("Setting 0% throttle.");
    current_throttle_us = 1500;
    update_esc_signal(current_throttle_us);
 
    Serial.println("Start");
}

// ============================================================================
// Main Loop 
// ============================================================================
void loop() {

    update_esc_signal(2000); 
}

// ============================================================================
// 1000us = 0% throttle, 1250us = 25% throttle, 1500us = 50% throttle, 1750us = 75% throttle, 2000us = 100% throttle (almost)