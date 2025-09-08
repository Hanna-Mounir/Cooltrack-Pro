#include <OneWire.h> 

#include <DallasTemperature.h> 

 

// Assign pins for each sensor 

#define FRIDGE_PIN 2 

#define FREEZER_PIN 3 

 

// Create OneWire instances for each pin 

OneWire oneWireFridge(FRIDGE_PIN); 

OneWire oneWireFreezer(FREEZER_PIN); 

 

// Pass the OneWire references to DallasTemperature 

DallasTemperature fridgeSensor(&oneWireFridge); 

DallasTemperature freezerSensor(&oneWireFreezer); 

 

void setup() { 

  Serial.begin(9600); 

 

  // Start both sensors 

  fridgeSensor.begin(); 

  freezerSensor.begin(); 

} 

 

void loop() { 

  // Request temperature readings 

  fridgeSensor.requestTemperatures(); 

  freezerSensor.requestTemperatures(); 

 

  // Read temperatures in Celsius 

  float tempFridge = fridgeSensor.getTempCByIndex(0); 

  float tempFreezer = freezerSensor.getTempCByIndex(0); 

 

  // Display in Serial Monitor 

  Serial.print("Fridge Temp: "); 

  Serial.print(tempFridge); 

  Serial.println(" °C"); 

 

  Serial.print("Freezer Temp: "); 

  Serial.print(tempFreezer); 

  Serial.println(" °C"); 

 

  Serial.println("----------------------"); 

  delay(2000); 

} 