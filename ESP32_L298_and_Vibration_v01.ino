/*
   This sketch is to start and run a motor in 2 modes: Normal operation mode and Maintainance mode
   It will record and output the accelerometer and gyro data of an attached MPU6510 sensor

   Written for Digital Labs project - G. Albrecht Jan 2022
*/ 
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Motor 1 pins
int iMotor1Pin1 = 27;
int iMotor1Pin2 = 26;
int iEnable1Pin = 14;
int iDutyCycle = 200;
int iMotorSpeed = 0; 

const int ciFreq = 30000;
const int ciPwmChannel = 0;
const int ciResolution = 8;

Adafruit_MPU6050 oMpu; // Instance of MPU6050

void setup() {
  Serial.begin(115200); // Define serial for output
  
  // Init of Motor Control ------------------
  // set output pins from ESP32 to L298N
  pinMode(iMotor1Pin1, OUTPUT);
  pinMode(iMotor1Pin2, OUTPUT);
  pinMode(iEnable1Pin, OUTPUT);
  // configure LED PWM functionalitites
  ledcSetup(ciPwmChannel, ciFreq, ciResolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(iEnable1Pin, ciPwmChannel);

  // Init of MPU6050 accelerometer ----------
  if (!oMpu.begin()) {
    Serial.println("Failed to find MPU6050 chip"); // Endless loop, can't find MPU
    while (1) {
      delay(10);
    }
  }
  // MPU found, define ranges
  oMpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  oMpu.setGyroRange(MPU6050_RANGE_250_DEG);
  oMpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println(""); //First empty line on plotter
  delay(100);  
}

void loop() {
  // Get sensor reading (we can also get the ambient temperature)
  sensors_event_t a, g, temp;
  oMpu.getEvent(&a, &g, &temp);

  // CSV output on plotter
  Serial.print("Acc-X:");
  Serial.print(a.acceleration.x + 8.0);
  Serial.print(",Acc-Y:");
  Serial.print(a.acceleration.y);
  Serial.print(",Acc-Z:");
  Serial.print(a.acceleration.z);
  Serial.print(",Gyro-X:");
  Serial.print(g.gyro.x);
  Serial.print(",Gyro-Y:");
  Serial.print(g.gyro.y);
  Serial.print(",Gyro-Z:");
  Serial.print(g.gyro.z);
  Serial.println("");

  // Read from Serial to control motor speed
  if (Serial.available() > 0) {
    iMotorSpeed = Serial.readString().toInt(); // read the incoming data
    //Serial.print("Input was: "); Serial.println(iMotorSpeed);
  }
  
  // Set Motorspeed by input
  if (iMotorSpeed < 0){ // Backward
    digitalWrite(iMotor1Pin1, HIGH);
    digitalWrite(iMotor1Pin2, LOW);
  }
  if (iMotorSpeed == 0){ // Stop
    digitalWrite(iMotor1Pin1, LOW);
    digitalWrite(iMotor1Pin2, LOW);
  } 
  if (iMotorSpeed > 0) { // Forward
    digitalWrite(iMotor1Pin1, LOW);
    digitalWrite(iMotor1Pin2, HIGH);
  }
  ledcWrite(ciPwmChannel, abs(iMotorSpeed)); // Rev speed in PWM output
  delay(5);
}
