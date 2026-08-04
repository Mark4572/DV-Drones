#include <SPI.h>
#include <Arduino.h>

// ============================================
// BMI270 ESP32 SPI Driver
// Reads accelerometer and gyroscope data
// ============================================

// ============= PIN CONFIGURATION =============
#define BMI270_CS_PIN    5   // Chip Select (GPIO5)
#define SPI_CLK_PIN      18  // Clock (GPIO18)
#define SPI_MOSI_PIN     23  // Master Out Slave In (GPIO23)
#define SPI_MISO_PIN     19  // Master In Slave Out (GPIO19)

// ============= BMI270 REGISTER MAP =============
#define BMI270_CHIP_ID           0x00
#define BMI270_ERR_REG           0x02
#define BMI270_STATUS            0x03
#define BMI270_AUX_X_LSB         0x04
#define BMI270_AUX_X_MSB         0x05
#define BMI270_AUX_Y_LSB         0x06
#define BMI270_AUX_Y_MSB         0x07
#define BMI270_AUX_Z_LSB         0x08
#define BMI270_AUX_Z_MSB         0x09
#define BMI270_ACCEL_X_LSB       0x0C
#define BMI270_ACCEL_X_MSB       0x0D
#define BMI270_ACCEL_Y_LSB       0x0E
#define BMI270_ACCEL_Y_MSB       0x0F
#define BMI270_ACCEL_Z_LSB       0x10
#define BMI270_ACCEL_Z_MSB       0x11
#define BMI270_GYRO_X_LSB        0x12
#define BMI270_GYRO_X_MSB        0x13
#define BMI270_GYRO_Y_LSB        0x14
#define BMI270_GYRO_Y_MSB        0x15
#define BMI270_GYRO_Z_LSB        0x16
#define BMI270_GYRO_Z_MSB        0x17
#define BMI270_ACCEL_CONFIG      0x20
#define BMI270_GYRO_CONFIG       0x21
#define BMI270_PWR_CONF          0x7C
#define BMI270_PWR_CTRL          0x7D

// SPI Settings
SPISettings spiSettings(10000000, MSBFIRST, SPI_MODE0); // 10 MHz, MSB first, Mode 0

// ============= FUNCTION PROTOTYPES =============
void spiInit();
uint8_t spiRead(uint8_t reg);
void spiWrite(uint8_t reg, uint8_t value);
void bmi270Init();
void bmi270ReadAccel(float &x, float &y, float &z);
void bmi270ReadGyro(float &x, float &y, float &z);
int16_t combine16bit(uint8_t lsb, uint8_t msb);
void printSensorData();

// ============= SPI COMMUNICATION FUNCTIONS =============

/**
 * Initialize SPI bus for ESP32
 */
void spiInit() {
  pinMode(BMI270_CS_PIN, OUTPUT);
  digitalWrite(BMI270_CS_PIN, HIGH);
  
  SPI.begin(SPI_CLK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, BMI270_CS_PIN);
  Serial.println("[SPI] Bus initialized");
}

/**
 * Read a single register from BMI270
 * SPI protocol: first byte is register address with read bit (0x80)
 */
uint8_t spiRead(uint8_t reg) {
  uint8_t value = 0;
  
  SPI.beginTransaction(spiSettings);
  digitalWrite(BMI270_CS_PIN, LOW);
  delayMicroseconds(1);
  
  // Send register address with read bit set (MSB = 1)
  SPI.transfer(reg | 0x80);
  delayMicroseconds(1);
  
  // Read data
  value = SPI.transfer(0x00);
  
  delayMicroseconds(1);
  digitalWrite(BMI270_CS_PIN, HIGH);
  SPI.endTransaction();
  
  return value;
}

/**
 * Write a single register to BMI270
 * SPI protocol: first byte is register address with write bit (0x00)
 */
void spiWrite(uint8_t reg, uint8_t value) {
  SPI.beginTransaction(spiSettings);
  digitalWrite(BMI270_CS_PIN, LOW);
  delayMicroseconds(1);
  
  // Send register address with write bit clear (MSB = 0)
  SPI.transfer(reg & 0x7F);
  delayMicroseconds(1);
  
  // Write data
  SPI.transfer(value);
  
  delayMicroseconds(1);
  digitalWrite(BMI270_CS_PIN, HIGH);
  SPI.endTransaction();
}

// ============= BMI270 INITIALIZATION =============

/**
 * Initialize BMI270 sensor
 * Sets up power management, accelerometer, and gyroscope
 */
void bmi270Init() {
  delay(100); // Startup delay
  
  // Check chip ID
  uint8_t chipID = spiRead(BMI270_CHIP_ID);
  Serial.print("[BMI270] Chip ID: 0x");
  Serial.println(chipID, HEX);
  
  if (chipID != 0x24) {
    Serial.println("[BMI270] ERROR: Invalid chip ID! Expected 0x24");
    return;
  }
  
  Serial.println("[BMI270] Chip ID verified!");
  
  // Power Management: Enable accelerometer and gyroscope
  // PWR_CTRL register: bits [7:6] = acc_en, [1:0] = gyro_en
  spiWrite(BMI270_PWR_CTRL, 0x0E); // Enable ACC and GYRO
  delay(50);
  
  // Configure Accelerometer
  // ACCEL_CONFIG: range ±8g (bits [1:0] = 01), ODR 100Hz (bits [5:2] = 0110)
  spiWrite(BMI270_ACCEL_CONFIG, 0x68); // ±8g range, 100 Hz ODR
  delay(10);
  
  // Configure Gyroscope
  // GYRO_CONFIG: range ±500 dps (bits [1:0] = 00), ODR 100Hz (bits [5:2] = 0110)
  spiWrite(BMI270_GYRO_CONFIG, 0x60); // ±500 dps range, 100 Hz ODR
  delay(10);
  
  Serial.println("[BMI270] Initialization complete!");
}

// ============= DATA READING FUNCTIONS =============

/**
 * Combine two 8-bit register values into a 16-bit signed integer
 * BMI270 uses little-endian format (LSB first)
 */
int16_t combine16bit(uint8_t lsb, uint8_t msb) {
  int16_t value = (int16_t)((msb << 8) | lsb);
  return value;
}

/**
 * Read accelerometer data and convert to m/s²
 * Range: ±8g, Sensitivity: 2048 LSB/g
 * 1g = 9.81 m/s²
 */
void bmi270ReadAccel(float &x, float &y, float &z) {
  uint8_t accel_x_lsb = spiRead(BMI270_ACCEL_X_LSB);
  uint8_t accel_x_msb = spiRead(BMI270_ACCEL_X_MSB);
  
  uint8_t accel_y_lsb = spiRead(BMI270_ACCEL_Y_LSB);
  uint8_t accel_y_msb = spiRead(BMI270_ACCEL_Y_MSB);
  
  uint8_t accel_z_lsb = spiRead(BMI270_ACCEL_Z_LSB);
  uint8_t accel_z_msb = spiRead(BMI270_ACCEL_Z_MSB);
  
  int16_t accel_x_raw = combine16bit(accel_x_lsb, accel_x_msb);
  int16_t accel_y_raw = combine16bit(accel_y_lsb, accel_y_msb);
  int16_t accel_z_raw = combine16bit(accel_z_lsb, accel_z_msb);
  
  // Convert to g (range ±8g, 2048 LSB/g)
  float accel_x_g = accel_x_raw / 2048.0f;
  float accel_y_g = accel_y_raw / 2048.0f;
  float accel_z_g = accel_z_raw / 2048.0f;
  
  // Convert to m/s²
  x = accel_x_g * 9.81f;
  y = accel_y_g * 9.81f;
  z = accel_z_g * 9.81f;
}

/**
 * Read gyroscope data and convert to degrees/second
 * Range: ±500 dps, Sensitivity: 65.5 LSB/dps
 */
void bmi270ReadGyro(float &x, float &y, float &z) {
  uint8_t gyro_x_lsb = spiRead(BMI270_GYRO_X_LSB);
  uint8_t gyro_x_msb = spiRead(BMI270_GYRO_X_MSB);
  
  uint8_t gyro_y_lsb = spiRead(BMI270_GYRO_Y_LSB);
  uint8_t gyro_y_msb = spiRead(BMI270_GYRO_Y_MSB);
  
  uint8_t gyro_z_lsb = spiRead(BMI270_GYRO_Z_LSB);
  uint8_t gyro_z_msb = spiRead(BMI270_GYRO_Z_MSB);
  
  int16_t gyro_x_raw = combine16bit(gyro_x_lsb, gyro_x_msb);
  int16_t gyro_y_raw = combine16bit(gyro_y_lsb, gyro_y_msb);
  int16_t gyro_z_raw = combine16bit(gyro_z_lsb, gyro_z_msb);
  
  // Convert to dps (range ±500 dps, 65.5 LSB/dps)
  x = gyro_x_raw / 65.5f;
  y = gyro_y_raw / 65.5f;
  z = gyro_z_raw / 65.5f;
}

/**
 * Read and print all sensor data
 */
void printSensorData() {
  float accel_x, accel_y, accel_z;
  float gyro_x, gyro_y, gyro_z;
  
  bmi270ReadAccel(accel_x, accel_y, accel_z);
  bmi270ReadGyro(gyro_x, gyro_y, gyro_z);
  
  Serial.println("===== BMI270 SENSOR DATA =====");
  
  Serial.print("Acceleration [m/s²]: ");
  Serial.print("X="); Serial.print(accel_x, 2);
  Serial.print(" Y="); Serial.print(accel_y, 2);
  Serial.print(" Z="); Serial.println(accel_z, 2);
  
  Serial.print("Gyroscope [dps]:     ");
  Serial.print("X="); Serial.print(gyro_x, 2);
  Serial.print(" Y="); Serial.print(gyro_y, 2);
  Serial.print(" Z="); Serial.println(gyro_z, 2);
  
  Serial.println("==============================\n");
}

// ============= SETUP & LOOP =============

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n[ESP32] Starting BMI270 SPI Driver...\n");
  
  // Initialize SPI bus
  spiInit();
  delay(100);
  
  // Initialize BMI270 sensor
  bmi270Init();
  delay(100);
}

void loop() {
  printSensorData();
  delay(1000); // Read every 1 second
}
