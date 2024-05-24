#include <ArduinoJson.h>
#include "Measurement.h"

void logValues(
  Measurement m,
  const char* errorMsgs[],
  int errorCount) {

  StaticJsonDocument<200> doc;

  doc["pH"] = round(m.pH * 100.0) / 100.0;
  doc["cels"] = m.temperature;
  doc["free"] = m.free_memory_bytes;
  doc["volt"] = round(m.voltage) / 1000.0;
  doc["msg"] = m.classified;

  JsonArray errors = doc.createNestedArray("errors");
  for (int i = 0; i < errorCount; i++) {
    errors.add(errorMsgs[i]);
  }

  serializeJson(doc, Serial);
  Serial.println();
}