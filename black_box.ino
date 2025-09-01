#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <SD.h>

// GPS setup
SoftwareSerial gpsSerial(4, 3); // RX, TX for GPS
TinyGPSPlus gps;

// BMP180 setup
Adafruit_BMP085 bmp;

// SD setup
const int chipSelect = 4;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);

  // BMP180 initialization
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP180 sensor!");
    while (1);
  }

  // SD Card initialization
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }

  Serial.println("Setup complete!");

  // Create CSV file with headers
  File dataFile = SD.open("data.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Latitude,Longitude,Temperature (°C),Pressure (Pa)"); // CSV headers
    dataFile.close();
  } else {
    Serial.println("Error opening file.");
  }
}

void loop() {
  updateGPS();
  logSensorData();
  delay(1000); // 1-second delay for readability
}

// Function to read and update GPS data
void updateGPS() {
  unsigned long startTime = millis();
  while (millis() - startTime < 2000) { // Timeout to avoid infinite loops
    while (gpsSerial.available()) {
      gps.encode(gpsSerial.read());
    }
  }

  if (gps.location.isValid()) {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
  } else {
    Serial.println("Waiting for valid GPS signal...");
  }
}

// Function to read BMP180 data and log all data to SD card
void logSensorData() {
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" Pa");

  // Open file and log data if GPS location is valid
  File dataFile = SD.open("data.csv", FILE_WRITE);
  if (dataFile) {
    if (gps.location.isValid()) {
      dataFile.print(gps.location.lat(), 6);
      dataFile.print(",");
      dataFile.print(gps.location.lng(), 6);
    } else {
      dataFile.print("NO_FIX,NO_FIX");
    }

    dataFile.print(",");
    dataFile.print(temperature);
    dataFile.print(",");
    dataFile.println(pressure);

    dataFile.close();
    Serial.println("Data written to SD card.");
  } else {
    Serial.println("Error opening file.");
  }
}
