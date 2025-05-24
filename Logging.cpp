#include <ArduinoJson.h>
#include "Measurement.h"


void logValues(
  Measurement m,
  const char* errorMsgs[],
  int errorCount,
  unsigned long accumulatedRuntime) {

  StaticJsonDocument<200> doc;

  doc["pH"] = round(m.pH * 100.0) / 100.0;
  doc["free_mem"] = m.free_memory_bytes;
  doc["mV"] = round(m.voltage);
  doc["msg"] = m.classified;
  doc["runtime"] = accumulatedRuntime / 1000;

  JsonObject temps = doc.createNestedObject("temperatures");

  for (int i = 0; i < m.tempCount; i++) {
    temps[String(i)] = round(m.allTemperatures[i] * 100.0) / 100.0;
  }

  JsonArray errors = doc.createNestedArray("errors");
  for (int i = 0; i < errorCount; i++) {
    errors.add(errorMsgs[i]);
  }

  serializeJson(doc, Serial);
  Serial.println();
}


void logSTARTUP() {

  StaticJsonDocument<200> doc;
  doc["msg"] = "s3SNOR1OOP init";

  serializeJson(doc, Serial);
  Serial.println();
}



