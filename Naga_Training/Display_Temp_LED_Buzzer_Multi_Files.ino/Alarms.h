#pragma once
#include <Arduino.h>

// Initialize LED pins and buzzer (off)
void initAlarms();

// Direct LED control (bools are ON/OFF states)
void setLeds(bool green, bool blue, bool red);

// Evaluate thresholds and update LEDs/buzzer + LCD status
void updateAlarms(float tempC, float ch4ppm);
