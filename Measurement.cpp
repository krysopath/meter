#include <Arduino.h>
#include "TemperatureSensor.h"
#include "PH_Sensor.h"

#define PH_ANALOG_mV 5000

extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;


int freeMemory() {
  int free_memory;
  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  } else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
    free_memory += ((int)__brkval) - ((int)&__heap_start);
  }
  return free_memory;
}

bool validateTemperature(float temperature, const char* errorMsgs[], int& errorCount) {
  if (isnan(temperature)) {
    errorMsgs[errorCount++] = "T Probe gone!";
    return false;
  }
  if (temperature < -55.0 || temperature > 125.0) {
    errorMsgs[errorCount++] = "T out of bounds!";
    return false;
  }
  return true;
}

bool validatePH(float phValue, const char* errorMsgs[], int& errorCount) {
  if (isnan(phValue)) {
    errorMsgs[errorCount++] = "pH is NaN!";
    return false;
  }
  if (phValue < 0 || phValue > 14) {
    errorMsgs[errorCount++] = "pH Probe is off!";
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

struct Measurement {
  float voltage;
  float pH;
  float temperature;
  int free_memory_bytes;
  char* classified;
};

Measurement NewMeasurement(int pin) {
  Measurement m;
  
  m.voltage = analogRead(pin) / 1023.0 * PH_ANALOG_mV;
  m.temperature = readTemperature();
  m.pH = readPHValue(m.voltage, m.temperature);
  Serial.println(m.pH);
  m.classified = describePH(m.pH);
  m.free_memory_bytes = freeMemory();
  return m;
}