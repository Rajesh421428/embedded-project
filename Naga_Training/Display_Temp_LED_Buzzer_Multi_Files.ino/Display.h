#pragma once
#include <Arduino.h>

// Initialize LCD and show initial text
void initDisplay();

// Show the measured values on line 0, and a basic CH4 label on line 1 before status
void printValuesToLCD(float t, float m);

// Overwrite line 1 with a short status message
void showStatusOnLCD(const char* status);

// Convenience to display parse error with the offending line on LCD
void lcdShowParseError(const char* line);
