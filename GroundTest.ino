#include <HX711.h>                 // Library for load cell by Bogdan
#include <SD.h>                    
#include <SPI.h>                   
                                   
// Pin Definitions                 
const int LOADCELL_DOUT_PIN = 3;   
const int LOADCELL_SCK_PIN = 2;    
const int SD_PIN = 4;              // pin for SD card
const int COUNTDOWN_PIN = 7;       // Pin to start the countdown
const int BUZZER_PIN = 6;          
const int LED_PIN = 5;             
const int MOSFET_PIN = 8;          // pin to control ignition

const float SCALE_FACTOR = 2280.f;
/*
We need to calibrate the scale to get accurate KG reading so we have to do the following. Then replace the above SCALE_FACTOR with the result.(ONLY DONE FIRST TIME)

1. Call set_scale() with no parameter.
2. Call tare() with no parameter.
3. Place a known weight on the scale and call get_units(10).
4. Divide the result in step 3 to your known weight. You should get about the parameter you need to pass to set_scale().
5. Adjust the parameter in step 4 until you get an accurate reading.
*/

HX711 loadCell;
File dataFile;

void countdown() 
{
  // 10-second countdown
  for (int i = 10; i > 0; i--) {
    Serial.println(i);
    delay(1000);

    if (i <= 3) {
      // Flash LED and beep buzzer at 3 seconds left
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(LED_PIN, LOW);
      delay(500);
    }
  }

  // Trigger ignition
  Serial.println("Ignition sequence initiated!");
  digitalWrite(MOSFET_PIN, HIGH);
}


void setup() 
{
  // Initialize communication for debugging
  Serial.begin(9600);

  // Initialize the load cell
  loadCell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  loadCell.set_scale(SCALE_FACTOR); 
  loadCell.tare();  // Reset scale to zero

  // Initialize SD card
  if (!SD.begin(SD_PIN)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  // Initialize pins
  pinMode(COUNTDOWN_PIN, INPUT_PULLUP);   // Use internal pull-up resistor
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);       // Ensure MOSFET is off at the start

  // Wait for button press to start countdown
  Serial.println("Waiting for button press...");
  while (digitalRead(COUNTDOWN_PIN) == HIGH) {
    delay(100);
  }

  // begin countdown
  countdown();
}


void loop() {
  // Read load cell data
  float weight = loadCell.get_units();

  // Print load cell reading
  Serial.print("Weight (Kg): ");
  Serial.println(weight);

  // Open the file in append mode and store the data
  dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.print("Weight (Kg): ");
    dataFile.println(weight);
    dataFile.close();
  } else {
    Serial.println("Error opening datalog.csv");
  }

  delay(100); // Loop delay to read load cell data multiple times per second
}
