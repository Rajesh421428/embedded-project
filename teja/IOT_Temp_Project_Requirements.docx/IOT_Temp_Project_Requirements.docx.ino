#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Change to 0x3F if your module uses that address
LiquidCrystal_I2C lcd(0x27, 16, 2);

const float TEMP_HI_C  = 30.0;
const float CH4_HI_PPM = 1000.0;

enum State { OKAY, WARN_TEMP, WARN_CH4, CRIT_BOTH };

// Parse "25.3,900" or "T=25.3 CH4=900" etc.
bool parseTwoNumbers(const String& in, float& outT, float& outCH4) {
  String s = in;
  for (size_t i = 0; i < s.length(); ++i) {
    char c = s[i];
    bool num = (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '+';
    if (!num) s.setCharAt(i, ' ');
  }
  s.trim();
  int sp = s.indexOf(' ');
  if (sp < 0) return false;
  String a = s.substring(0, sp); a.trim();
  String b = s.substring(sp + 1); b.trim();
  if (!a.length() || !b.length()) return false;
  outT   = a.toFloat();
  outCH4 = b.toFloat();
  return !(isnan(outT) || isnan(outCH4));
}

State evalState(float t, float ch4) {
  bool th = (t > TEMP_HI_C);
  bool ch = (ch4 > CH4_HI_PPM);
  if (th && ch) return CRIT_BOTH;
  if (th)       return WARN_TEMP;
  if (ch)       return WARN_CH4;
  return OKAY;
}

void printLCD(float t, float ch4, State s) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");  lcd.print(t, 1); lcd.print("C ");
  lcd.print("C:");  lcd.print((long)ch4); // C=CH4 ppm (short label)

  lcd.setCursor(0, 1);
  switch (s) {
    case OKAY:      lcd.print("State: OK");    break;
    case WARN_TEMP: lcd.print("WARN: TEMP");   break;
    case WARN_CH4:  lcd.print("WARN: CH4");    break;
    case CRIT_BOTH: lcd.print("CRIT: BOTH");   break;
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  Serial.println("\nEnter readings as 'temp,ppm' (e.g. 25.3,900)");
  Serial.println("Also ok: 'T=25.3 CH4=900'");

  lcd.begin();       // your library: no args
  lcd.backlight();   // if backlight doesn't toggle, try lcd.setBacklight(HIGH)
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Enter: temp,ppm");
  lcd.setCursor(0, 1); lcd.print("e.g. 25.0,900");
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n'); line.trim();
    if (!line.length()) return;

    float t, ch4;
    if (parseTwoNumbers(line, t, ch4)) {
      State s = evalState(t, ch4);

      // Serial echo
      Serial.print("T="); Serial.print(t, 2); Serial.print(" C, ");
      Serial.print("CH4="); Serial.print(ch4, 0); Serial.print(" ppm -> ");
      Serial.println(s==OKAY ? "OK" : s==WARN_TEMP ? "WARN TEMP" : s==WARN_CH4 ? "WARN CH4" : "CRIT BOTH");

      // LCD update
      printLCD(t, ch4, s);
    } else {
      Serial.println("Parse error. Try: 25.0,900");
      lcd.clear(); lcd.print("Bad input!");
    }
  }
}
