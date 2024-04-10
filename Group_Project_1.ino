//SD Card
#include <SPI.h>
#include <SD.h>

#include <Wire.h>
#include <MPU6050.h> //Imports MPU6050 library assuming it is the standard one by Electronic Cats

#define LOG_FILE_NAME "rocket_data.csv" // Name of the log file on the SD card

const int digitalPin = 10; //Variable for what pin the SD card module is connected to on the Arduino

File dataFile; //File to write and log data to a file on SD card
MPU6050 mpu; //Initalizes an accelerometer object

void setup() {
  Wire.begin();
  Serial.begin(9600); //Starts serial communication so that Arduino can send commands through USB; 9600 is baud rate (speed)

  while (!Serial) {
    // Wait for serial port to connect
    ;
  }

//Checks if the SD card successfuly connects to Arduino
  Serial.print("Initializing SD card...");
  if (!SD.begin(digitalPin)) {
    Serial.println("Initialization failed!");
    return; //Terminates program if not able to connect to the SD card
  }
  Serial.println("Initialization done.");


//Checks if the Accelerometer successfuly connects to Arduino
  Serial.println("Initializing MPU6050...");
  if (!mpu.begin()) {
    Serial.println("Failed to initialize MPU6050!");
    return; //Terminates program if not able to connect to the accelerometer
  }
  Serial.println("MPU6050 initialized successfully!");

  /*Set accelerometer range using predefined constant for accuracy. You can use (+/-) 2g, 4g, 8g, and 16g. */
  mpu.setFullScaleRange(MPU6050_ACCEL_FS_8);

  dataFile = SD.open(LOG_FILE_NAME, FILE_WRITE);
  if (!dataFile) {
    Serial.println("Error opening accelerometer log file!");
    return;
  }
  Serial.println("Log file opened successfully.");

  dataFile.close();
}


void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az); //Retreieves acceleration info
  
  sensors_event_t event;
  event.acceleration.x = ax;
  event.acceleration.y = ay;
  event.acceleration.z = az;
  
  logData(event);
  delay(100);

 /*
  Alternative code for retrieving accelerometer and gyroscope info if using the Adafruit_ADXL345_U library

  Vector3f accel = mpu.readAccelerometer();
  Vector3f gyro = mpu.readGyroscope();

  Serial.print("Accelerometer: ");
  Serial.print("X = ");
  Serial.print(accel.x);
  Serial.print(", Y = ");
  Serial.print(accel.y);
  Serial.print(", Z = ");
  Serial.println(accel.z);

  Serial.print("Gyroscope: ");
  Serial.print("X = ");
  Serial.print(gyro.x);
  Serial.print(", Y = ");
  Serial.print(gyro.y);
  Serial.print(", Z = ");
  Serial.println(gyro.z);
*/
}

//Function to log data such as accelrometer into a file
void logData(sensors_event_t event) {
  //Checks if you can access log
  if (!dataFile) {
    Serial.println("Error opening log file!");
    return;
  }

  unsigned long currentTime = millis(); // Retrieves the current time of accelerometer data logging

  dataFile.println("Time (ms), Acceleration X (m/s^2), Acceleration Y (m/s^2), Acceleration Z (m/s^2)");

  //Writes the current time and accelerometer data to the log file.
  dataFile.print(currentTime);
  dataFile.print(", ");
  dataFile.print(event.acceleration.x);
  dataFile.print(", ");
  dataFile.print(event.acceleration.y);
  dataFile.print(", ");
  dataFile.println(event.acceleration.z);

  dataFile.flush(); //Flushes the data to ensure it is immediately written to the SD card.
}

