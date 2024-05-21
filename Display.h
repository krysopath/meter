#ifndef DISPLAY_H
#define DISPLAY_H

void initializeLCD();
void displayValues(float phValue, float temperature, const char* classified, const char* errorMsgs[], int errorCount);


#endif  // DISPLAY_H
