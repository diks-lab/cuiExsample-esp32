#include <Arduino.h>

#include "cuiConfig.h"

void setup()
{
  Serial.begin(115200);
  delay(200);

  cfg.init();
}

void loop()
{
  if (Serial.available() > 0)
  {
    if (ENTER_KEY == Serial.read()) // [Enter]キーでConfigモードへ移行
    {
      cfg.run();

      cls();
      Serial << "\033[1;1HExits Config mode.\033[0K";
      delay(2000);
      Serial << "\033[1;1HRun mode.\033[0K";
    }
  }
}
