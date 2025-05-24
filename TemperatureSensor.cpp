#include <OneWire.h>
#include "TemperatureSensor.h"
#include <Arduino.h>

#include "max6675.h"

int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

#define DS18S20_Pin A2

OneWire ds(DS18S20_Pin);

float readTemperatureMX6675() {
  return thermocouple.readCelsius();
} 

float readTemperature() {
  byte data[12];
  byte addr[8];

  if (!ds.search(addr)) {
    ds.reset_search();
    return NAN;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return NAN;
  }

  if (addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized, addr: ");
    Serial.println(addr[0], HEX);
    return NAN;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);

  ds.reset();
  ds.select(addr);
  ds.write(0xBE);

  for (int i = 0; i < 9; i++) {
    data[i] = ds.read();
  }

  ds.reset_search();

  int16_t rawTemperature = (data[1] << 8) | data[0];
  return rawTemperature / 16.0;
}
