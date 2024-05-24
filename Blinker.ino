#include "Measurement.h"
#include "Logging.h"
#include "TemperatureSensor.h"
#include "PH_Sensor.h"
#include "Display.h"
#include <avr/wdt.h>
#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#define WAKEUP_TIMER RTC_PERIOD_CYC8192_gc

#define PH_PIN A1
#define SOFT_RESET true
#define MAIN_UPDATE 1000U

// const int free_memory_bytes;
unsigned long previousMillis = 0;

Measurement m;

void softReset() {
  wdt_enable(WDTO_15MS);
  while (1)
    ;
}

void RTC_init(void) {
  while (RTC.STATUS > 0)
    ; /* Wait for all register to be synchronized */

  PORTE.DIRSET = PIN2_bm;

  RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;                      // set internal clock @ 1000hz Hz
  RTC.PITINTCTRL = RTC_PI_bm;                            // enable PIT interrupt
  RTC.PITCTRLA = RTC_PERIOD_CYC16384_gc | RTC_PITEN_bm;  // interrupt frq: 2 Hz
  //RTC.CTRLA = RTC_PRESCALER_DIV2_gc | RTC_RTCEN_bm;
}

ISR(RTC_PIT_vect) {
  RTC.PITINTFLAGS = RTC_PI_bm;  // Clear interrupt flag by writing '1' (required)
  PORTE.OUTTGL = PIN2_bm;       // toggle PE2
}

void initializeSerial() {
  Serial.begin(9600);
}

void blink() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);
  digitalWrite(LED_BUILTIN, LOW);
}

void setup() {
  digitalWrite(LED_BUILTIN, LOW);

  initializeLCD();
  initializeSerial();
  initializePH();

  m.voltage = 1500;
  //m.pH = 7;
  m.temperature = 25;

  pinMode(LED_BUILTIN, OUTPUT);

  RTC_init();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Set sleep mode to POWER DOWN mode
  sleep_enable();
}

void loop() {

  const char* errorMsgs[5];
  int errorCount = 0;

  bool reset_now = false;
  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis > MAIN_UPDATE) {
    previousMillis = millis();

    if (SOFT_RESET && reset_now) {
      softReset();
    }

    m = NewMeasurement(PH_PIN);
    Serial.println(m.pH);
    if (!validateTemperature(
          m.temperature,
          errorMsgs, errorCount)) {
      //temperature = 25.0;
    }
    if (!validatePH(
          m.pH,
          errorMsgs, errorCount)) {
      //phValue = NAN;
    }

    displayValues(m, errorMsgs, errorCount);
    logValues(m, errorMsgs, errorCount);
  }
  calibratePH(m.voltage, m.temperature);


  blink();
  //sleep_cpu();
}