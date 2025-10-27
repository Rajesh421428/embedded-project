#pragma once
#include <Arduino.h>

// Extract the first two numbers found anywhere in the string.
// Returns true if both numbers were found.
bool extractTwoNumbers(const char* str, float& a, float& b);
