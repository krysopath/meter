#include "TemperatureSensor.h"
#include "PH_Sensor.h"
#include "Display.h"
#include "Memory.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <avr/wdt.h>

#define PH_PIN A1
#define PH_ANALOG_mV 5000
#define SOFT_RESET true
#define MAIN_UPDATE 1000U

float voltage, phValue, temperature = 25;
int free_memory_bytes;
unsigned long previousMillis = 0;


void initializeSerial() {
  Serial.begin(115200);
}

void setup() {
  initializeLCD();
  initializeSerial();
  initializePH();
}

void loop() {

  free_memory_bytes = freeMemory();
  bool reset_now = false;
  int errorCount = 0;
  const char* classified;
  const char* errorMsgs[5];


  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > MAIN_UPDATE) {
    previousMillis = millis();
    voltage = analogRead(PH_PIN) / 1023.0 * PH_ANALOG_mV;


    temperature = readTemperature();
    if (!validateTemperature(temperature, errorMsgs, errorCount)) {
      //temperature = 25.0;
    }

    phValue = readPHValue(voltage, temperature);
    if (!validatePH(phValue, errorMsgs, errorCount)) {
      //phValue = NAN;
    }

    if (free_memory_bytes < 512) {
      errorMsgs[errorCount++] = "Memory < 512";
      reset_now = true;
    }
    classified = describePH(phValue);

    if (SOFT_RESET && reset_now) {
      softReset();
    }

    displayValues(phValue, temperature, classified, errorMsgs, errorCount);
    logValues(phValue, temperature, free_memory_bytes, voltage, classified, errorMsgs, errorCount);
  }
  calibratePH(voltage, temperature);
}

void softReset() {
  wdt_enable(WDTO_15MS);
  while (1)
    ;
}

void logValues(
  float phValue,
  float temperature,
  int free_memory_bytes,
  float voltage,
  const char* classified,
  const char* errorMsgs[],
  int errorCount) {

  StaticJsonDocument<200> doc;

  doc["pH"] = round(phValue * 100.0) / 100.0;
  doc["cels"] = temperature;
  doc["free"] = free_memory_bytes;
  doc["volt"] = round(voltage) / 1000.0;
  doc["msg"] = classified;



  // Add error messages to the JSON document
  JsonArray errors = doc.createNestedArray("errors");
  for (int i = 0; i < errorCount; i++) {
    errors.add(errorMsgs[i]);
  }



  serializeJson(doc, Serial);
  Serial.println();
}

bool validateTemperature(float temperature, const char* errorMsgs[], int& errorCount) {
  if (isnan(temperature)) {
    errorMsgs[errorCount++] = "T is NaN";
    return false;
  }
  if (temperature < -55.0 || temperature > 125.0) {
    errorMsgs[errorCount++] = "T out of bounds";
    return false;
  }
  return true;
}

bool validatePH(float phValue, const char* errorMsgs[], int& errorCount) {
  if (isnan(phValue)) {
    errorMsgs[errorCount++] = "pH is NaN";
    return false;
  }
  if (phValue < 0 || phValue > 14) {
    errorMsgs[errorCount++] = "Probe gone";
    return false;
  }
  return true;
}

const char* describePH(float phValue) {
  if (phValue < 0 || phValue > 14) {
    return "Invalid pH";
  } else if (phValue < 3.0) {
    return "Strongly Acidic";
  } else if (phValue < 6.0) {
    return "Weakly Acidic";
  } else if (phValue < 8.0) {
    return "Neutral";
  } else if (phValue < 11.0) {
    return "Weakly Alkaline";
  } else {
    return "Strongly Alkaline";
  }
}
