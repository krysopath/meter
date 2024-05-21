#include <LiquidCrystal_I2C.h>
#include <math.h>
#include "Display.h"
#include <Arduino.h>

#define I2C_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_LINES 2

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

void initializeLCD() {
  lcd.init();
  lcd.clear();
  lcd.backlight();
}

void displayValues(
  float phValue, 
  float temperature, 
  const char* classified, 
  const char* errorMsgs[], 
  int errorCount) {

  static int scrollIndex = 0;
  char lcd_mesg0[16];
  char lcd_mesg1[64] = "";
  char buffer_ph[8];
  char buffer_temp[8];

  strncat(lcd_mesg1, classified, sizeof(lcd_mesg1) - strlen(lcd_mesg1) - 1);

  for (int i = 0; i < errorCount; i++) {
    strncat(lcd_mesg1, ", ", sizeof(lcd_mesg1) - strlen(lcd_mesg1) - 1);
    strncat(lcd_mesg1, errorMsgs[i], sizeof(lcd_mesg1) - strlen(lcd_mesg1) - 1);
  }

  if (isnan(temperature)) {
    temperature = 25.0;
  }

  dtostrf(temperature, 5, 1, buffer_temp);
  dtostrf(phValue, 5, 1, buffer_ph);
  snprintf(lcd_mesg0, sizeof(lcd_mesg0), "pH%s @%sC", buffer_ph, buffer_temp);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcd_mesg0);

  if (strlen(lcd_mesg1) > 16) {
    char scrollBuffer[17];
    strncpy(scrollBuffer, lcd_mesg1 + scrollIndex, 16);
    scrollBuffer[16] = '\0';
    lcd.setCursor(0, 1);
    lcd.print(scrollBuffer);
    scrollIndex++;
    if (scrollIndex > strlen(lcd_mesg1) - 16) {
      scrollIndex = 0;
    }
  } else {
    lcd.setCursor(0, 1);
    lcd.print(lcd_mesg1);
  }
}
