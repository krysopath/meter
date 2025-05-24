#ifndef THERMOCOUPLEMANAGER_H
#define THERMOCOUPLEMANAGER_H

#include <max6675.h>

class ThermocoupleSensor {
  public:
    ThermocoupleSensor(int clk, int cs, int doPin)
      : thermocouple(clk, cs, doPin), csPin(cs),
        rawAt0C(0.0), rawAt100C(100.0) {}

    void begin() {}

    float readRaw() {
      return thermocouple.readCelsius();
    }

    float getCalibratedTemperature() {
      float raw = readRaw();
      // Apply linear correction based on calibration points
      float scale = (100.0 - 0.0) / (rawAt100C - rawAt0C);
      float offset = -rawAt0C * scale;
      return raw * scale + offset;
    }

    void setCalibration(float rawAt0, float rawAt100) {
      rawAt0C = rawAt0;
      rawAt100C = rawAt100;
    }

  private:
    MAX6675 thermocouple;
    int csPin;
    float rawAt0C;
    float rawAt100C;
};

class ThermocoupleManager {
  public:
    ThermocoupleManager(int clk, int doPin) : clkPin(clk), doPin(doPin), count(0) {}

    void addSensor(int csPin) {
      if (count < MAX_SENSORS) {
        sensors[count] = new ThermocoupleSensor(clkPin, csPin, doPin);
        sensors[count]->begin();
        count++;
      }
    }

    float readSensor(int index) {
      if (index < 0 || index >= count) return NAN;
      return sensors[index]->getCalibratedTemperature();
    }

    void calibrateSensor(int index, float rawAt0, float rawAt100) {
      if (index < 0 || index >= count) return;
      sensors[index]->setCalibration(rawAt0, rawAt100);
    }

    int sensorCount() const { return count; }

  private:
    static const int MAX_SENSORS = 8;
    ThermocoupleSensor* sensors[MAX_SENSORS];
    int clkPin, doPin;
    int count;
};

#endif
