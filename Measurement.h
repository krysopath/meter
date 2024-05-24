#ifndef MEASUREMENT_H
#define MEASUREMENT_H

bool validateTemperature(float temperature, const char* errorMsgs[], int& errorCount);
bool validatePH(float phValue, const char* errorMsgs[], int& errorCount);
const char* describePH(float phValue);

struct Measurement {
  float voltage;
  float pH;
  float temperature;
  int free_memory_bytes;
  char* classified;
};

Measurement NewMeasurement(int pin);


#endif  // MEASUREMENT_H
