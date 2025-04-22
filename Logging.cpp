#include <ArduinoJson.h>
#include "Measurement.h"

void logValues(
  Measurement m,
  const char* errorMsgs[],
  int errorCount) {

  StaticJsonDocument<200> doc;

  doc["pH"] = round(m.pH * 100.0) / 100.0;
  doc["celsius"] = round(m.temperature * 100.0) / 100.0;
  doc["free_mem"] = m.free_memory_bytes;
  doc["mV"] = round(m.voltage);
  doc["msg"] = m.classified;

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



