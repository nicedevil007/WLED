# 1 "C:\\Users\\manuel\\AppData\\Local\\Temp\\tmpxyodj9wz"
#include <Arduino.h>
# 1 "C:/Users/manuel/Documents/GitHub/WLED/wled00/wled00.ino"
# 13 "C:/Users/manuel/Documents/GitHub/WLED/wled00/wled00.ino"
#include "wled.h"
void setup();
void loop();
#line 15 "C:/Users/manuel/Documents/GitHub/WLED/wled00/wled00.ino"
void setup() {
  WLED::instance().setup();
}

void loop() {
  WLED::instance().loop();
}