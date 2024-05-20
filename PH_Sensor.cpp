#include "DFRobot_PH.h"
#include <EEPROM.h>
#include "PH_Sensor.h"
#define N 10  // Number of samples for the moving average

float samples[N];
int sampleIndex = 0;
float sum = 0;
int count = 0;        

DFRobot_PH ph;

void initializePH() {
  ph.begin();
  for (int i = 0; i < N; i++) {
    samples[i] = 0;
  }
}

float addSample(float sample) {
  if (count == N) {
    sum -= samples[sampleIndex];
  } else {
    count++;
  }

  sum += sample;
  samples[sampleIndex] = sample;

  sampleIndex = (sampleIndex + 1) % N;
  return sum / count;
}

float readPHValue(float voltage, float temperature) {
  float newSample = ph.readPH(voltage, temperature);
  return addSample(newSample);
}

void calibratePH(float voltage, float temperature) {
  ph.calibration(voltage, temperature);
}

