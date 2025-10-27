#include "Alarms.h"
#include "Config.h"
#include "Display.h"

static inline void ledWrite(int pin, bool on) {
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

void initAlarms() {
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN,  OUTPUT);
  pinMode(LED_RED_PIN,   OUTPUT);
  pinMode(BUZZER_PIN,    OUTPUT);

  setLeds(false, false, false);
  noTone(BUZZER_PIN);
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
