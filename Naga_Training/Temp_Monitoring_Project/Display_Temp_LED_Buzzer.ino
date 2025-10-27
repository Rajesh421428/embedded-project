#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Change to 0x3F if your LCD address differs

// ---------- User settings ----------
const unsigned long USB_BAUD = 9600;

// Thresholds
const float TEMP_HIGH = 50.0;    // °C
const float CH4_HIGH  = 1000.0;  // ppm

// LED pins (either 3 discrete LEDs or an RGB LED's channels)
const int LED_GREEN_PIN = 6;
const int LED_BLUE_PIN  = 5;
const int LED_RED_PIN   = 7;

// If you use a COMMON ANODE RGB LED, set this to true to invert logic
const bool RGB_COMMON_ANODE = false;

// Buzzer (passive) pin
const int BUZZER_PIN = 8;
const int BUZZER_HZ  = 2000;     // tone frequency for alarm

// Serial line buffer
char lineBuf[64];
size_t idx = 0;
unsigned long lastCharTime = 0;

// Last parsed values
float lastTemp = NAN;
float lastCH4  = NAN;

// ---------- Helpers ----------
inline void ledWrite(int pin, bool on) {
  if (RGB_COMMON_ANODE) {
    digitalWrite(pin, on ? LOW : HIGH);
  } else {
    digitalWrite(pin, on ? HIGH : LOW);
  }
}

void setLeds(bool green, bool blue, bool red) {
  ledWrite(LED_GREEN_PIN, green);
  ledWrite(LED_BLUE_PIN,  blue);
  ledWrite(LED_RED_PIN,   red);
}

void showStatusOnLCD(const char* status) {
  // Line 0 already shows numbers; put status on line 1 after CH4 or replace as needed
  // Here we overwrite line 1 entirely with status text for clarity.
  lcd.setCursor(0, 1);
  lcd.print("                "); // clear line 1
  lcd.setCursor(0, 1);
  lcd.print(status);
}

void updateAlarms(float t, float m) {
  bool tempHigh = (!isnan(t) && t >= TEMP_HIGH);
  bool ch4High  = (!isnan(m) && m >= CH4_HIGH);

  if (tempHigh && ch4High) {
    // Both out of range: RED + buzzer
    setLeds(false, false, true);
    tone(BUZZER_PIN, BUZZER_HZ);
    showStatusOnLCD("DANGER: TEMP & CH4");
  } else if (tempHigh || ch4High) {
    // Exactly one out of range: BLUE, no buzzer
    setLeds(false, true, false);
    noTone(BUZZER_PIN);
    if (tempHigh) showStatusOnLCD("ALERT: TEMP HIGH");
    else          showStatusOnLCD("ALERT: CH4 HIGH");
  } else {
    // All good: GREEN, no buzzer
    setLeds(true, false, false);
    noTone(BUZZER_PIN);
    showStatusOnLCD("GOOD: IN RANGE");
  }
}

bool extractTwoNumbers(const char* str, float &a, float &b) {
  int count = 0;
  const char* p = str;
  char* end;

  while (*p != '\0' && count < 2) {
    if ((*p >= '0' && *p <= '9') || *p == '-' || *p == '+') {
      float val = strtod(p, &end);
      if (end != p) {
        if (count == 0) a = val; else b = val;
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

void printValuesToLCD(float t, float m) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(t, 1);
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("CH4 :");
  lcd.print(m, 0);
  lcd.print(" ppm");
}

void handleLine(const char* s) {
  float t = NAN, m = NAN;

  // Robust parsing: picks first two numbers anywhere in the line
  bool ok = extractTwoNumbers(s, t, m);

  if (ok) {
    lastTemp = t;
    lastCH4  = m;

    // Show values
    printValuesToLCD(t, m);
    // Immediately update status LEDs/buzzer + status message (overwrites line 1)
    updateAlarms(t, m);

    // Echo to serial
    Serial.print("Parsed OK -> Temp: ");
    Serial.print(t, 2);
    Serial.print(" C, Methane: ");
    Serial.print(m, 0);
    Serial.println(" ppm");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Parse error");
    lcd.setCursor(0, 1);
    lcd.print(s);

    // Failsafe: set BLUE as a warning (ambiguous), silence buzzer
    setLeds(false, true, false);
    noTone(BUZZER_PIN);

    Serial.print("Parse error: ");
    Serial.println(s);
  }
}

void setup() {
  Serial.begin(USB_BAUD);

  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN,  OUTPUT);
  pinMode(LED_RED_PIN,   OUTPUT);
  pinMode(BUZZER_PIN,    OUTPUT);

  // Default: all off, buzzer off
  setLeds(false, false, false);
  noTone(BUZZER_PIN);

  lcd.begin();   // Your LiquidCrystal_I2C variant uses begin() with no args
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting data...");
  lcd.setCursor(0, 1);
  lcd.print("Baud 9600");
}

void loop() {
  bool gotLine = false;

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

  // Allow parsing if the sender pauses typing
  if (idx > 0 && (millis() - lastCharTime > 300)) {
    gotLine = true;
  }

  if (gotLine) {
    lineBuf[idx] = '\0';
    handleLine(lineBuf);
    idx = 0;
  }
}
