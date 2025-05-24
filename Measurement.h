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

  float allTemperatures[8];
  int tempCount;
};

Measurement NewMeasurement(int analogPin);


#endif  // MEASUREMENT_H
