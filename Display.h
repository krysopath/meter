#ifndef DISPLAY_H
#define DISPLAY_H

#include "Measurement.h"

void initializeLCD();
const char* formatRuntime(unsigned long ms);
void displayValues(Measurement m, const char* errorMsgs[], int errorCount, unsigned long accumulatedRuntime);

#endif  // DISPLAY_H
