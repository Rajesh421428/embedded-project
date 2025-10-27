#include "Parser.h"
#include <stdlib.h>

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
