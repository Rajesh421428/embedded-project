#include "Config.h"
#include "Parser.h"
#include "Display.h"
#include "Alarms.h"

char lineBuf[64];
size_t idx = 0;
unsigned long lastCharTime = 0;

float lastTemp = NAN;
float lastCH4  = NAN;

static void handleLine(const char* s) {
  float t = NAN, m = NAN;

  if (extractTwoNumbers(s, t, m)) {
    lastTemp = t;
    lastCH4  = m;

    printValuesToLCD(t, m);   // line 0 values
    updateAlarms(t, m);       // LEDs/buzzer + status on line 1

    Serial.print("Parsed OK -> Temp: ");
    Serial.print(t, 2);
    Serial.print(" C, Methane: ");
    Serial.print(m, 0);
    Serial.println(" ppm");
  } else {
    lcdShowParseError(s);

    // Failsafe: BLUE warning, buzzer off
    setLeds(false, true, false);
    noTone(BUZZER_PIN);

    Serial.print("Parse error: ");
    Serial.println(s);
  }
}

void setup() {
  Serial.begin(USB_BAUD);

  initDisplay();     // LCD init + splash
  initAlarms();      // pinModes, all LEDs off, buzzer off

  Serial.println("\nSend 'temp,ppm' like: 25.3,900");
  Serial.println("Freeform is OK too: T=25.3 CH4=900");
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

  // Parse after short pause in typing
  if (idx > 0 && (millis() - lastCharTime > 300)) {
    gotLine = true;
  }

  if (gotLine) {
    lineBuf[idx] = '\0';
    handleLine(lineBuf);
    idx = 0;
  }
}
