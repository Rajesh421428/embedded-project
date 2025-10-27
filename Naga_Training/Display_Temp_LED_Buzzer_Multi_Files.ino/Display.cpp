#include "Display.h"
#include "Config.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Local LCD instance (module internal)
static LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

void initDisplay() {
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Waiting data...");
  lcd.setCursor(0, 1); lcd.print("Baud "); lcd.print(USB_BAUD);
}

void printValuesToLCD(float t, float m) {
  lcd.clear();

  // Line 0: Temp
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(t, 1);
  lcd.print((char)223);
  lcd.print("C");

  // Line 1: CH4 value (will be replaced by status shortly)
  lcd.setCursor(0, 1);
  lcd.print("CH4 :");
  lcd.print(m, 0);
  lcd.print(" ppm");
}

void showStatusOnLCD(const char* status) {
  // Overwrite line 1 with status text
  lcd.setCursor(0, 1);
  lcd.print("                "); // clear line 1
  lcd.setCursor(0, 1);
  lcd.print(status);
}

void lcdShowParseError(const char* line) {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Parse error");
  lcd.setCursor(0, 1);
  // Truncate long lines to fit
  for (uint8_t i = 0; i < LCD_COLS && line[i]; ++i) lcd.print(line[i]);
}
