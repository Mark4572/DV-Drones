// ============= ADVANCED IMU270 FEATURES =============
// Add this to your main sketch for enhanced functionality
// Includes: calibration, low-pass filtering, motion detection

#include <vector>

// ============= CALIBRATION DATA =============
struct CalibrationData {
  float accelOffsetX = 0.0, accelOffsetY = 0.0, accelOffsetZ = 0.0;
  float gyroOffsetX = 0.0, gyroOffsetY = 0.0, gyroOffsetZ = 0.0;
  bool isCalibrated = false;
};

// ============= LOW-PASS FILTER =============
class LowPassFilter {
private:
  float alpha;  // Filter coefficient (0.0 to 1.0)
  float prevValue;
  
public:
  LowPassFilter(float cutoffFreq, float sampleRate) {
    // Calculate alpha from cutoff frequency
    // alpha = 2*pi*cutoffFreq / (2*pi*cutoffFreq + sampleRate)
    alpha = cutoffFreq / (cutoffFreq + sampleRate);
    prevValue = 0.0;
  }
  
  float filter(float newValue) {
    prevValue = (alpha * newValue) + ((1.0 - alpha) * prevValue);
    return prevValue;
  }
};

// ============= MOTION DETECTION =============
class MotionDetector {
private:
  float accelerationThreshold;
  float angularVelocityThreshold;
  
public:
  MotionDetector(float accelThresh = 0.5, float gyroThresh = 10.0) 
    : accelerationThreshold(accelThresh), angularVelocityThreshold(gyroThresh) {}
  
  bool detectMotion(const IMUData& data) {
    // Calculate total acceleration magnitude
    float accelMag = sqrt(data.accelX * data.accelX + 
                          data.accelY * data.accelY + 
                          data.accelZ * data.accelZ);
    
    // Calculate total angular velocity magnitude
    float gyroMag = sqrt(data.gyroX * data.gyroX + 
                         data.gyroY * data.gyroY + 
                         data.gyroZ * data.gyroZ);
    
    // Subtract 1g from acceleration (gravity)
    accelMag = abs(accelMag - 1.0);
    
    return (accelMag > accelerationThreshold || 
            gyroMag > angularVelocityThreshold);
  }
};

// ============= QUATERNION-BASED ORIENTATION =============
class Quaternion {
public:
  float w, x, y, z;
  
  Quaternion() : w(1.0), x(0.0), y(0.0), z(0.0) {}
  Quaternion(float _w, float _x, float _y, float _z) 
    : w(_w), x(_x), y(_y), z(_z) {}
  
  // Normalize quaternion
  void normalize() {
    float magnitude = sqrt(w*w + x*x + y*y + z*z);
    if (magnitude == 0.0) return;
    w /= magnitude;
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
  }
  
  // Convert to Euler angles (radians)
  void toEuler(float& roll, float& pitch, float& yaw) {
    // Roll (X-axis rotation)
    float sinr_cosp = 2 * (w * x + y * z);
    float cosr_cosp = 1 - 2 * (x * x + y * y);
    roll = atan2(sinr_cosp, cosr_cosp);
    
    // Pitch (Y-axis rotation)
    float sinp = sqrt(1 + 2 * (w * y - z * x));
    float cosp = sqrt(1 - 2 * (w * y - z * x));
    pitch = 2 * atan2(sinp, cosp) - M_PI / 2;
    
    // Yaw (Z-axis rotation)
    float siny_cosp = 2 * (w * z + x * y);
    float cosy_cosp = 1 - 2 * (y * y + z * z);
    yaw = atan2(siny_cosp, cosy_cosp);
  }
};

// ============= MADGWICK FUSION ALGORITHM =============
class MadgwickFilter {
private:
  Quaternion q;
  float beta;  // Algorithm gain
  float sampleRate;
  
public:
  MadgwickFilter(float rate = 10.0, float gain = 0.1) 
    : sampleRate(rate), beta(gain) {}
  
  void update(float gx, float gy, float gz, float ax, float ay, float az) {
    // Convert gyroscope from deg/s to rad/s
    gx *= 0.0174533;  // pi/180
    gy *= 0.0174533;
    gz *= 0.0174533;
    
    // Normalize accelerometer
    float norm = sqrt(ax*ax + ay*ay + az*az);
    if (norm == 0) return;
    ax /= norm;
    ay /= norm;
    az /= norm;
    
    // Algorithm from Madgwick paper
    float halfvx = (q.y*gz - q.z*gy);
    float halfvy = (q.z*gx - q.x*gz);
    float halfvz = (q.x*gy - q.y*gx);
    
    float halfex = (ay*q.z - az*q.y);
    float halfey = (az*q.x - ax*q.z);
    float halfez = (ax*q.y - ay*q.x);
    
    gx = gx + beta * (2.0 * halfex);
    gy = gy + beta * (2.0 * halfey);
    gz = gz + beta * (2.0 * halfez);
    
    // Integrate quaternion
    float dt = 1.0 / sampleRate;
    q.w = q.w + (-q.x*gx - q.y*gy - q.z*gz) * dt;
    q.x = q.x + (q.w*gx + q.y*gz - q.z*gy) * dt;
    q.y = q.y + (q.w*gy - q.x*gz + q.z*gx) * dt;
    q.z = q.z + (q.w*gz + q.x*gy - q.y*gx) * dt;
    
    q.normalize();
  }
  
  void getEulerAngles(float& roll, float& pitch, float& yaw) {
    q.toEuler(roll, pitch, yaw);
    // Convert to degrees
    roll *= 57.2958;
    pitch *= 57.2958;
    yaw *= 57.2958;
  }
  
  Quaternion getQuaternion() {
    return q;
  }
};

// ============= CALIBRATION ROUTINE =============
void calibrateIMU(CalibrationData& calibData, int samples = 100) {
  Serial.println("\n===== IMU270 CALIBRATION =====");
  Serial.println("Place IMU on level surface and DO NOT MOVE for 5 seconds...");
  delay(5000);
  
  float sumAccelX = 0, sumAccelY = 0, sumAccelZ = 0;
  float sumGyroX = 0, sumGyroY = 0, sumGyroZ = 0;
  
  Serial.print("Calibrating (collecting ");
  Serial.print(samples);
  Serial.println(" samples)...");
  
  for (int i = 0; i < samples; i++) {
    readIMU270();
    sumAccelX += imuData.accelX;
    sumAccelY += imuData.accelY;
    sumAccelZ += imuData.accelZ;
    sumGyroX += imuData.gyroX;
    sumGyroY += imuData.gyroY;
    sumGyroZ += imuData.gyroZ;
    
    if ((i + 1) % 10 == 0) {
      Serial.print(".");
    }
    delay(50);
  }
  
  // Calculate averages
  float avgAccelX = sumAccelX / samples;
  float avgAccelY = sumAccelY / samples;
  float avgAccelZ = sumAccelZ / samples;
  float avgGyroX = sumGyroX / samples;
  float avgGyroY = sumGyroY / samples;
  float avgGyroZ = sumGyroZ / samples;
  
  // Calculate offsets
  calibData.accelOffsetX = avgAccelX;
  calibData.accelOffsetY = avgAccelY;
  calibData.accelOffsetZ = avgAccelZ - 1.0;  // Remove 1g gravity
  calibData.gyroOffsetX = avgGyroX;
  calibData.gyroOffsetY = avgGyroY;
  calibData.gyroOffsetZ = avgGyroZ;
  calibData.isCalibrated = true;
  
  Serial.println("\n\nCalibration Complete!");
  Serial.print("Accel Offset: X=");
  Serial.print(calibData.accelOffsetX, 4);
  Serial.print(" Y=");
  Serial.print(calibData.accelOffsetY, 4);
  Serial.print(" Z=");
  Serial.println(calibData.accelOffsetZ, 4);
  Serial.print("Gyro Offset: X=");
  Serial.print(calibData.gyroOffsetX, 2);
  Serial.print(" Y=");
  Serial.print(calibData.gyroOffsetY, 2);
  Serial.print(" Z=");
  Serial.println(calibData.gyroOffsetZ, 2);
}

// ============= APPLY CALIBRATION =============
void applyCalibratedData(IMUData& data, const CalibrationData& calibData) {
  if (!calibData.isCalibrated) return;
  
  data.accelX -= calibData.accelOffsetX;
  data.accelY -= calibData.accelOffsetY;
  data.accelZ -= calibData.accelOffsetZ;
  data.gyroX -= calibData.gyroOffsetX;
  data.gyroY -= calibData.gyroOffsetY;
  data.gyroZ -= calibData.gyroOffsetZ;
}

// ============= DATA LOGGING STRUCTURE =============
struct DataLog {
  unsigned long timestamp;
  float accelX, accelY, accelZ;
  float gyroX, gyroY, gyroZ;
  float temperature;
  float roll, pitch, yaw;
};

// ============= STATISTICS CALCULATOR =============
class SensorStatistics {
private:
  std::vector<float> values;
  const int maxSize = 1000;
  
public:
  void addValue(float value) {
    values.push_back(value);
    if (values.size() > maxSize) {
      values.erase(values.begin());
    }
  }
  
  float getMean() {
    if (values.empty()) return 0.0;
    float sum = 0.0;
    for (float v : values) sum += v;
    return sum / values.size();
  }
  
  float getStdDev() {
    if (values.empty()) return 0.0;
    float mean = getMean();
    float sumSquareDiff = 0.0;
    for (float v : values) {
      float diff = v - mean;
      sumSquareDiff += diff * diff;
    }
    return sqrt(sumSquareDiff / values.size());
  }
  
  float getMin() {
    if (values.empty()) return 0.0;
    return *std::min_element(values.begin(), values.end());
  }
  
  float getMax() {
    if (values.empty()) return 0.0;
    return *std::max_element(values.begin(), values.end());
  }
  
  void clear() {
    values.clear();
  }
};

// ============= USAGE EXAMPLE =============
/*
// In your setup():
CalibrationData calibData;
LowPassFilter accelXFilter(10.0, 10.0);  // 10Hz cutoff, 10Hz sample rate
MadgwickFilter orientation(10.0);
MotionDetector motionDetector(0.3, 15.0);

// Calibrate once at startup
calibrateIMU(calibData);

// In your loop():
readIMU270();
applyCalibratedData(imuData, calibData);

// Apply low-pass filter
imuData.accelX = accelXFilter.filter(imuData.accelX);

// Update orientation
orientation.update(imuData.gyroX, imuData.gyroY, imuData.gyroZ,
                   imuData.accelX, imuData.accelY, imuData.accelZ);

// Get Euler angles
float roll, pitch, yaw;
orientation.getEulerAngles(roll, pitch, yaw);

// Check for motion
if (motionDetector.detectMotion(imuData)) {
  Serial.println("Motion detected!");
}
*/