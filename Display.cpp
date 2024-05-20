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

// void displayValues(float phValue, float temperature) {
//   char lcd_mesg0[16];
//   char lcd_mesg1[16];
//   char buffer[8];

//   if (isnan(phValue)) {
//     snprintf(lcd_mesg0, sizeof(lcd_mesg0), "pH Error");
//   } else {
//     dtostrf(phValue, 5, 2, buffer);
//     snprintf(lcd_mesg0, sizeof(lcd_mesg0), "pH%s", buffer);
//   }

//   if (isnan(temperature)) {
//     snprintf(lcd_mesg1, sizeof(lcd_mesg1), "t = Error");
//   } else {
//     dtostrf(temperature, 5, 1, buffer);
//     snprintf(lcd_mesg1, sizeof(lcd_mesg1), "t = %s C", buffer);
//   }

//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print(lcd_mesg0);
//   lcd.setCursor(0, 1);
//   lcd.print(lcd_mesg1);
// }

// void displayValues(float phValue, float temperature) {
//   char lcd_mesg0[16];
//   char lcd_mesg1[16] = "";
//   char buffer_ph[8];
//   char buffer_temp[8];

//   if (isnan(phValue) && isnan(temperature)) {
//     snprintf(lcd_mesg0, sizeof(lcd_mesg0), "");
//     snprintf(lcd_mesg1, sizeof(lcd_mesg1), "pH & t Error");
//   } else if (isnan(phValue)) {
//     snprintf(lcd_mesg0, sizeof(lcd_mesg0), "t=");
//     dtostrf(temperature, 5, 1, buffer_temp);
//     strncat(lcd_mesg0, buffer_temp, sizeof(lcd_mesg0) - strlen(lcd_mesg0) - 1);
//     strncat(lcd_mesg0, " C", sizeof(lcd_mesg0) - strlen(lcd_mesg0) - 1);
//     snprintf(lcd_mesg1, sizeof(lcd_mesg1), "pH Error");
//   } else if (isnan(temperature)) {
//     snprintf(lcd_mesg0, sizeof(lcd_mesg0), "pH");
//     dtostrf(phValue, 5, 1, buffer_ph);
//     strncat(lcd_mesg0, buffer_ph, sizeof(lcd_mesg0) - strlen(lcd_mesg0) - 1);
//     strncat(lcd_mesg0, " t=25C", sizeof(lcd_mesg0) - strlen(lcd_mesg0) - 1);
//     snprintf(lcd_mesg1, sizeof(lcd_mesg1), "t Error");
//   } else {
//     dtostrf(phValue, 5, 1, buffer_ph);
//     dtostrf(temperature, 5, 1, buffer_temp);
//     snprintf(lcd_mesg0, sizeof(lcd_mesg0), "pH%s t=%s C", buffer_ph, buffer_temp);
//   }

//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print(lcd_mesg0);
//   lcd.setCursor(0, 1);
//   lcd.print(lcd_mesg1);
// }

// void displayValues(float phValue, float temperature) {
//   char lcd_mesg0[16];
//   char lcd_mesg1[16] = "";
//   char buffer_ph[8];
//   char buffer_temp[8];
//   const char* errorMsgs[2];
//   int errorCount = 0;

//   // Validate pH value
//   if (isnan(phValue)) {

//     errorMsgs[errorCount++] = "pH Error";
//   }
//   if (phValue < 0 || phValue > 16) {
//     errorMsgs[errorCount++] = "pH Error";
//   }

//   // Validate temperature
//   if (isnan(temperature)) {
//     errorMsgs[errorCount++] = "t Error";
//     temperature = 25;
//   }

//   for (int i = 0; i < errorCount; i++) {
//     if (i > 0) {
//       strncat(lcd_mesg1, " & ", sizeof(lcd_mesg1) - strlen(lcd_mesg1) - 1);
//     }
//     strncat(lcd_mesg1, errorMsgs[i], sizeof(lcd_mesg1) - strlen(lcd_mesg1) - 1);
//   }

//   dtostrf(temperature, 5, 1, buffer_temp);
//   dtostrf(phValue, 5, 1, buffer_ph);
//   snprintf(lcd_mesg0, sizeof(lcd_mesg0), "pH%s @%sC", buffer_ph, buffer_temp);

//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print(lcd_mesg0);
//   lcd.setCursor(0, 1);
//   lcd.print(lcd_mesg1);
// }

void displayValues(float phValue, float temperature, const char* errorMsgs[], int errorCount) {
  static int scrollIndex = 0;
  char lcd_mesg0[16];
  char lcd_mesg1[32] = "";  // Increase size to handle scrolling
  char buffer_ph[8];
  char buffer_temp[8];

  // Construct the error message string
  for (int i = 0; i < errorCount; i++) {
    if (i > 0) {
      strncat(lcd_mesg1, " & ", sizeof(lcd_mesg1) - strlen(lcd_mesg1) - 1);
    }
    strncat(lcd_mesg1, errorMsgs[i], sizeof(lcd_mesg1) - strlen(lcd_mesg1) - 1);
  }
  if (isnan(temperature)) {
    temperature = 25;
  }

  // Format temperature and pH values for display
  dtostrf(temperature, 5, 1, buffer_temp);
  dtostrf(phValue, 5, 1, buffer_ph);
  snprintf(lcd_mesg0, sizeof(lcd_mesg0), "pH%s @%sC", buffer_ph, buffer_temp);

  // Display the first line message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcd_mesg0);

  // Scroll the error message if it's too long
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
