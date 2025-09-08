#include <Wire.h> 

#include <LiquidCrystal_I2C.h> 

#include <OneWire.h> 

#include <DallasTemperature.h> 

#include <RTClib.h> 

#include <SD.h> 

 

// ================== Pin Assignments ================== 

#define FRIDGE_PIN   2        // DS18B20 (Fridge) 

#define FREEZER_PIN  3        // DS18B20 (Freezer) 

 

#define SD_CS        10       // Chip Select for SD card 

 

// ================== Sensor Objects ================== 

OneWire oneWireFridge(FRIDGE_PIN); 

OneWire oneWireFreezer(FREEZER_PIN); 

DallasTemperature fridgeSensor(&oneWireFridge); 

DallasTemperature freezerSensor(&oneWireFreezer); 

 

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD (I²C address 0x27) 

RTC_DS3231 rtc; 

 

File logFile; 

 

// ================== Setup ================== 

void setup() { 

  Serial.begin(9600); 

 

  // Start temperature sensors 

  fridgeSensor.begin(); 

  freezerSensor.begin(); 

 

  // Start LCD 

  lcd.init(); 

  lcd.backlight(); 

  lcd.clear(); 

  lcd.setCursor(0, 0); 

  lcd.print("CoolTrack Pro"); 

  delay(2000); 

  lcd.clear(); 

 

  // Start RTC 

  if (!rtc.begin()) { 

    Serial.println("RTC not found!"); 

    lcd.print("RTC Fail"); 

    while (1); 

  } 

  if (rtc.lostPower()) { 

    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set to compile time 

  } 

 

  // Start SD card 

  if (!SD.begin(SD_CS)) { 

    Serial.println("SD Card init failed!"); 

    lcd.print("SD Fail"); 

    while (1); 

  } 

  Serial.println("SD Card Ready."); 

  lcd.print("SD Ready"); 

  delay(1000); 

  lcd.clear(); 

 

  // Prepare log file header 

  logFile = SD.open("temp_log.csv", FILE_WRITE); 

  if (logFile) { 

    logFile.println("Date,Time,Fridge_C,Freezer_C"); 

    logFile.close(); 

  } 

} 

 

// ================== Main Loop ================== 

void loop() { 

  // ----- Time ----- 

  DateTime now = rtc.now(); 

 

  // ----- Temperatures ----- 

  fridgeSensor.requestTemperatures(); 

  freezerSensor.requestTemperatures(); 

  float tempFridge = fridgeSensor.getTempCByIndex(0); 

  float tempFreezer = freezerSensor.getTempCByIndex(0); 

 

  // ----- Serial Monitor ----- 

  Serial.print(now.timestamp(DateTime::TIMESTAMP_DATE)); 

  Serial.print(" "); 

  Serial.print(now.timestamp(DateTime::TIMESTAMP_TIME)); 

  Serial.print(" | Fridge: "); 

  Serial.print(tempFridge); Serial.print(" °C | Freezer: "); 

  Serial.print(tempFreezer); Serial.println(" °C"); 

 

  // ----- LCD Output ----- 

  lcd.setCursor(0, 0); 

  lcd.print("Fridge:"); 

  lcd.print(tempFridge, 1); 

  lcd.print((char)223); 

  lcd.print("C   "); // padding to clear leftovers 

 

  lcd.setCursor(0, 1); 

  lcd.print("Freezer:"); 

  lcd.print(tempFreezer, 1); 

  lcd.print((char)223); 

  lcd.print("C   "); 

 

  // ----- Log to SD ----- 

  logFile = SD.open("temp_log.csv", FILE_WRITE); 

  if (logFile) { 

    logFile.print(now.year());   logFile.print("-"); 

    logFile.print(now.month());  logFile.print("-"); 

    logFile.print(now.day());    logFile.print(","); 

    logFile.print(now.hour());   logFile.print(":"); 

    logFile.print(now.minute()); logFile.print(":"); 

    logFile.print(now.second()); logFile.print(","); 

    logFile.print(tempFridge, 1); logFile.print(","); 

    logFile.println(tempFreezer, 1); 

    logFile.close(); 

    Serial.println("Data logged."); 

  } else { 

    Serial.println("Error opening log file!"); 

  } 

 

  delay(2000); // update every 2 sec 

} 

 