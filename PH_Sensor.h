#ifndef PH_SENSOR_H
#define PH_SENSOR_H

void initializePH();
float readPHValue(float voltage, float temperature);
void calibratePH(float voltage, float temperature);
void handleSerialCalibration(float voltage, float temperature, char* cmd);


#endif // PH_SENSOR_H
