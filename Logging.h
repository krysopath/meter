#ifndef LOGGING_H
#define LOGGING_H

#include "Measurement.h"

void logValues(
  Measurement m,
  const char* errorMsgs[],
  int errorCount);


void logSTARTUP();

#endif  // LOGGING_H
