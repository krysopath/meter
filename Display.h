#ifndef DISPLAY_H
#define DISPLAY_H

#include "Measurement.h"

void initializeLCD();
void displayValues(Measurement m, const char* errorMsgs[], int errorCount);

#endif  // DISPLAY_H
