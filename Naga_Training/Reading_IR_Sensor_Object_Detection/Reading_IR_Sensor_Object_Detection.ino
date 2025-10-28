#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------- User settings ----------------
const uint8_t LCD_ADDR = 0x27;   // try 0x3F if LCD is blank
const uint8_t LCD_COLS = 16;
const uint8_t LCD_ROWS = 2;

const uint8_t IR_PIN = 9;        // your chosen pin for IR OUT
const bool IR_ACTIVE_LOW = true; // most IR modules: LOW = detected

// ---------------- Objects ----------------
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// ---------------- Setup ----------------
void setup() {
  Serial.begin(9600);

  pinMode(IR_PIN, INPUT);        // use INPUT_PULLUP if output floats
  // pinMode(IR_PIN, INPUT_PULLUP);

  lcd.begin();                   // your library expects no arguments
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("IR Sensor Monitor");
  lcd.setCursor(0, 1); lcd.print("Initializing...");
  delay(1000);
}

// ---------------- Loop ----------------
void loop() {
  int state = digitalRead(IR_PIN);
  bool detected = IR_ACTIVE_LOW ? (state == LOW) : (state == HIGH);

  // --- Serial debug ---
  Serial.print("IR raw="); Serial.print(state);
  Serial.print("  detected="); Serial.println(detected ? "YES" : "NO");

  // --- LCD display ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IR Status:");

  lcd.setCursor(0, 1);
  if (detected) {
    lcd.print("Object DETECTED");
  } else {
    lcd.print("No Object     ");
  }

  delay(200);  // refresh 5× per second
}
