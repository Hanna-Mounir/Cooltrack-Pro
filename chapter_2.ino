#include <Wire.h> 

#include <LiquidCrystal_I2C.h> 

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

 

// Initialize the LCD (Address: 0x27 or 0x3F, Columns: 16, Rows: 2) 

LiquidCrystal_I2C lcd(0x27, 16, 2);  

 

void setup() { 

  Serial.begin(9600); 

 

  // Start sensors 

  fridgeSensor.begin(); 

  freezerSensor.begin(); 

 

  // Start LCD 

  lcd.init(); 

  lcd.backlight(); 

  lcd.clear(); 

  lcd.setCursor(0, 0); 

  lcd.print("CoolTrack Pro"); 

  delay(2000); // Show title for 2 sec 

} 

 

void loop() { 

  // Request temperatures 

  fridgeSensor.requestTemperatures(); 

  freezerSensor.requestTemperatures(); 

 

  // Read temperatures 

  float tempFridge = fridgeSensor.getTempCByIndex(0); 

  float tempFreezer = freezerSensor.getTempCByIndex(0); 

 

  // Serial Monitor output 

  Serial.print("Fridge Temp: "); 

  Serial.print(tempFridge); 

  Serial.println(" °C"); 

 

  Serial.print("Freezer Temp: "); 

  Serial.print(tempFreezer); 

  Serial.println(" °C"); 

  Serial.println("----------------------"); 

 

  // LCD output 

  lcd.clear(); 

  lcd.setCursor(0, 0); 

  lcd.print("Fridge: "); 

  lcd.print(tempFridge, 1); // 1 decimal place 

  lcd.print((char)223); // Degree symbol 

  lcd.print("C"); 

 

  lcd.setCursor(0, 1); 

  lcd.print("Freezer: "); 

  lcd.print(tempFreezer, 1); 

  lcd.print((char)223); 

  lcd.print("C"); 

 

  delay(2000); 

} 

 