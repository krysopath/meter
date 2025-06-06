
#include "Measurement.h"
#include "Logging.h"
#include "TemperatureSensor.h"
#include "PH_Sensor.h"
#include "Display.h"

#include <avr/wdt.h>
#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include <EEPROM.h>

#include "ThermocoupleManager.h"

// === Pins ===
const int thermoDO = 4;
const int thermoCLK = 6;
const int csPins[] = {5, 7};
const int phAnalogPin = A0;

// === Thermocouple Manager ===
ThermocoupleManager thermoManager(thermoCLK, thermoDO);


#define WAKEUP_TIMER RTC_PERIOD_CYC8192_gc
#define EEPROM_UPDATE_INTERVAL_ADDR 16


#define PH_PIN A1
#define DS18S20_Pin 2

#define BUTTON_PIN A3
#define MAX_ACCUMULATED_TIME (24UL * 60UL * 60UL * 1000UL) //24h

unsigned long accumulatedRuntime = 0;


#define SOFT_RESET true
#define MAIN_UPDATE 5000U


// const int free_memory_bytes;
unsigned long previousMillis = 0;
unsigned long mainUpdateInterval = 5000U;


Measurement m;

void softReset() {
  wdt_enable(WDTO_15MS);
  while (1)
    ;
}

void initializeSerial() {
  Serial.begin(115200);
}

void blink() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);
  digitalWrite(LED_BUILTIN, LOW);
}


void saveUpdateIntervalToEEPROM(unsigned long interval) {
  EEPROM.put(EEPROM_UPDATE_INTERVAL_ADDR, interval);
}

unsigned long loadUpdateIntervalFromEEPROM() {
  unsigned long storedInterval;
  EEPROM.get(EEPROM_UPDATE_INTERVAL_ADDR, storedInterval);
  // Sanity check to avoid garbage values on fresh EEPROM
  if (storedInterval < 100 || storedInterval > 60000) {
    storedInterval = 5000; // Default fallback
  }
  return storedInterval;
}


void setup() {
  digitalWrite(LED_BUILTIN, LOW);
  
  mainUpdateInterval = loadUpdateIntervalFromEEPROM();

  initializeLCD();
  initializeSerial();
  initializePH();

  m.voltage = 1500;
  //m.pH = 7;
  m.temperature = 25;

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  for (int i = 0; i < sizeof(csPins)/sizeof(csPins[0]); i++) {
    thermoManager.addSensor(csPins[i]);
  }
  // Optional: Manual calibration for each probe
  // Example: Raw values seen at ice point and boiling point
  thermoManager.calibrateSensor(0, 0, 100);  // Sensor 0
  thermoManager.calibrateSensor(1, 0, 100);  // Sensor 1

  logSTARTUP();
  
}

void printMenu() {
  Serial.println(F("\n--- MENU ---"));
  Serial.println(F("1: Enter pH Calibration Mode"));
  Serial.println(F("2: Calibrate (auto-detect 4.0 or 7.0)"));
  Serial.println(F("3: Exit Calibration Mode"));
  Serial.println(F("4: Soft Reset"));
  Serial.println(F("5: Set Update Interval (ms)"));
  Serial.println(F("?: Show Menu"));
  Serial.println(F("--------------\n"));
}

void handleSerialInput(float voltage, float temperature) {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toUpperCase();

    if (input == "1") {
      handleSerialCalibration(voltage, temperature, (char*)"ENTERPH");
    } else if (input == "2") {
      handleSerialCalibration(voltage, temperature, (char*)"CALPH");
    } else if (input == "3") {
      handleSerialCalibration(voltage, temperature, (char*)"EXITPH");
    } else if (input == "4") {
      Serial.println(F("Soft resetting..."));
      delay(100);
      softReset();
    } else if (input == "5") {
      Serial.println(F("Enter new update interval in milliseconds:"));
      while (Serial.available() == 0);  // wait for input
      String intervalStr = Serial.readStringUntil('\n');
      intervalStr.trim();
      unsigned long newInterval = intervalStr.toInt();
      if (newInterval > 500) {
        saveUpdateIntervalToEEPROM(newInterval);
        mainUpdateInterval = newInterval;
        Serial.print(F("Update interval set to "));
        Serial.print(mainUpdateInterval);
        Serial.println(F(" ms."));
      } else {
        Serial.println(F("Invalid input. Interval not changed."));
      }
    } else if (input == "?") {
      printMenu();
    } else {
      Serial.println(F("Invalid option. Type '?' to see the menu."));
    }
  }
}

void handleTime() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println(F("Button pressed — runtime counter reset."));
    accumulatedRuntime = 0;
  }

  if (accumulatedRuntime > MAX_ACCUMULATED_TIME) {
    accumulatedRuntime = 0;
  }
}

void loop() {

  const char* errorMsgs[5];
  int errorCount = 0;

  bool reset_now = false;
  unsigned long currentMillis = millis();

  handleSerialInput(m.voltage, m.temperature);


  if (currentMillis - previousMillis > mainUpdateInterval) {
    previousMillis = millis();

    accumulatedRuntime += mainUpdateInterval;
    handleTime();

    if (SOFT_RESET && reset_now) {
      softReset();
    }

    m = NewMeasurement(PH_PIN);

    if (!validateTemperature(
          m.temperature,
          errorMsgs, errorCount)) {
      m.temperature = 25.0;
    }
    if (!validatePH(
          m.pH,
          errorMsgs, errorCount)) {
    }

    displayValues(m, errorMsgs, errorCount, accumulatedRuntime);
    logValues(m, errorMsgs, errorCount, accumulatedRuntime);  
      
  }
  calibratePH(m.voltage, m.temperature);

  blink();
}