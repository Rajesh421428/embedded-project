#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Change 0x27 to 0x3F if your LCD uses that address
const unsigned long USB_BAUD = 9600;   // Set baud rate to 9600

char lineBuf[64];
size_t idx = 0;
unsigned long lastCharTime = 0;

void setup() {
  Serial.begin(USB_BAUD);
  lcd.begin();  // No arguments for your library
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting data...");
}

void loop() {
  bool gotLine = false;

  // Read incoming serial data
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    lastCharTime = millis();

    if (c == '\n' || c == '\r') {
      if (idx > 0) gotLine = true;
      break;
    } else if (idx < sizeof(lineBuf) - 1) {
      lineBuf[idx++] = c;
    }
  }

  // Auto-parse if user stops typing
  if (idx > 0 && (millis() - lastCharTime > 300)) {
    gotLine = true;
  }

  if (gotLine) {
    lineBuf[idx] = '\0';
    handleLine(lineBuf);
    idx = 0;
  }
}

void handleLine(const char* s) {
  float temp = NAN, methane = NAN;
  bool ok = extractTwoNumbers(s, temp, methane);

  lcd.clear();
  if (ok) {
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(temp, 1);
    lcd.print((char)223);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("CH4 :");
    lcd.print(methane, 0);
    lcd.print(" ppm");

    Serial.print("Parsed OK -> Temp: ");
    Serial.print(temp);
    Serial.print(" C, Methane: ");
    Serial.print(methane);
    Serial.println(" ppm");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Parse error");
    lcd.setCursor(0, 1);
    lcd.print(s);

    Serial.print("Parse error: ");
    Serial.println(s);
  }
}

// ✅ New function: extracts first two numbers in the string (very reliable)
bool extractTwoNumbers(const char* str, float &a, float &b) {
  int count = 0;
  const char* p = str;
  char* end;

  while (*p != '\0' && count < 2) {
    // Skip non-numeric chars
    if ((*p >= '0' && *p <= '9') || *p == '-' || *p == '+') {
      float val = strtod(p, &end);
      if (end != p) {
        if (count == 0) a = val;
        else b = val;
        count++;
        p = end;
      } else {
        p++;
      }
    } else {
      p++;
    }
  }

  return (count == 2);
}
