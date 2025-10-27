#pragma once
#include <Arduino.h>

// ---------- User settings ----------
constexpr unsigned long USB_BAUD = 9600;

// Thresholds
constexpr float TEMP_HIGH = 50.0f;   // °C
constexpr float CH4_HIGH  = 800.0f; // ppm

// LED pins (either 3 discrete LEDs or an RGB LED channels)
constexpr int LED_GREEN_PIN = 6;
constexpr int LED_BLUE_PIN  = 5;
constexpr int LED_RED_PIN   = 7;

// Common Anode RGB? (true=invert LED logic)
constexpr bool RGB_COMMON_ANODE = false;

// Buzzer (passive) pin + tone frequency
constexpr int BUZZER_PIN = 8;
constexpr int BUZZER_HZ  = 2000;

// LCD address and geometry
constexpr uint8_t LCD_I2C_ADDR = 0x27; // change to 0x3F if needed
constexpr uint8_t LCD_COLS = 16;
constexpr uint8_t LCD_ROWS = 2;
