#include "TemperatureSensor.h"
#include "PH_Sensor.h"
#include "Display.h"
#include "Memory.h"
#include <Arduino.h>
#include <ArduinoJson.h>

#define PH_PIN A1
#define PH_SUPPLY_mV 5000


float voltage, phValue, temperature = 25;
int free_memory_bytes;

void initializeSerial() {
  Serial.begin(115200);
}

void setup() {
  initializeLCD();
  initializeSerial();
  initializePH();
}

void loop() {
  static unsigned long timepoint = millis();
  free_memory_bytes = freeMemory();
  char errorMsg[32] = "";
  int errorCount = 0;


  const char* errorMsgs[3];

  if (free_memory_bytes < 100) {
  }

  if (millis() - timepoint > 1000U) {
    timepoint = millis();
    voltage = analogRead(PH_PIN) / 1023.0 * PH_SUPPLY_mV;


    temperature = readTemperature();
    if (!validateTemperature(temperature, errorMsgs, errorCount)) {
      //temperature = 25.0;
    }

    phValue = readPHValue(voltage, temperature);
    if (!validatePH(phValue, errorMsgs, errorCount)) {
      //phValue = NAN;
    }

    logValues(phValue, temperature, free_memory_bytes, voltage, errorMsgs, errorCount);
    displayValues(phValue, temperature, errorMsgs, errorCount);
  }

  calibratePH(voltage, temperature);
}


void logValues(
  float phValue,
  float temperature,
  int free_memory_bytes,
  float voltage,
  const char* errorMsgs[],
  int errorCount) {

  StaticJsonDocument<200> doc;

  doc["pH"] = round(phValue * 100.0) / 100.0;
  doc["cel"] = temperature;
  doc["free"] = free_memory_bytes;
  doc["voltage"] = round(voltage) / 1000.0;



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
    errorMsgs[errorCount++] = "t is NaN";
    return false;
  }
  if (temperature < -55.0 || temperature > 125.0) {
    errorMsgs[errorCount++] = "t out of bounds";
    return false;
  }
  return true;
}

bool validatePH(float phValue, const char* errorMsgs[], int& errorCount) {
  if (isnan(phValue)) {
    errorMsgs[errorCount++] = "pH is NaN";
    return false;
  }
  
  errorMsgs[errorCount++] = describePH(phValue);

  if (phValue < 0 || phValue > 14) {
    errorMsgs[errorCount++] = "probe gone";
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
