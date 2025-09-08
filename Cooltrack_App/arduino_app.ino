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
  Serial.begin(115200);  // Communication with ESP32 (changed to 115200)

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
    Serial.println("RTC_ERROR");
    lcd.print("RTC Fail");
    while (1);
  }
  

  // Start SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD_ERROR");
    lcd.print("SD Fail");
    while (1);
  }
  lcd.print("SD Ready");
  delay(1000);
  lcd.clear();

  // Prepare log file header (only if file doesn't exist)
  if (!SD.exists("temp_log.csv")) {
    logFile = SD.open("temp_log.csv", FILE_WRITE);
    if (logFile) {
      logFile.println("Date,Time,Fridge_C,Freezer_C");
      logFile.close();
    }
  }
  
  Serial.println("ARDUINO_READY");
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

  // ----- Send data to ESP32 via Serial -----
  // Format: TEMP,fridge_temp,freezer_temp,date,time
  Serial.print("TEMP,");
  Serial.print(tempFridge, 1);
  Serial.print(",");
  Serial.print(tempFreezer, 1);
  Serial.print(",");
  
  // Date in YYYY-MM-DD format
  Serial.print(now.year()); Serial.print("-");
  if (now.month() < 10) Serial.print("0");
  Serial.print(now.month()); Serial.print("-");
  if (now.day() < 10) Serial.print("0");
  Serial.print(now.day()); Serial.print(",");
  
  // Time in HH:MM:SS format
  if (now.hour() < 10) Serial.print("0");
  Serial.print(now.hour()); Serial.print(":");
  if (now.minute() < 10) Serial.print("0");
  Serial.print(now.minute()); Serial.print(":");
  if (now.second() < 10) Serial.print("0");
  Serial.print(now.second());
  Serial.println(); // End of data packet

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
    if (now.month() < 10) logFile.print("0");
    logFile.print(now.month());  logFile.print("-");
    if (now.day() < 10) logFile.print("0");
    logFile.print(now.day());    logFile.print(",");
    if (now.hour() < 10) logFile.print("0");
    logFile.print(now.hour());   logFile.print(":");
    if (now.minute() < 10) logFile.print("0");
    logFile.print(now.minute()); logFile.print(":");
    if (now.second() < 10) logFile.print("0");
    logFile.print(now.second()); logFile.print(",");
    logFile.print(tempFridge, 1); logFile.print(",");
    logFile.println(tempFreezer, 1);
    logFile.close();
  }

  delay(2000); // update every 2 sec
}
