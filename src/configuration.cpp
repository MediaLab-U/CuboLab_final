#include "configuration.h"
boolean modoConfig = true;
boolean firstConfig = true;
Preferences preferences;


void initMemory() {
  preferences.begin("MediaLab", false);
}

void configFileSystem() {
  LittleFS.begin();
}

void configTime(){
  
  struct tm timeinfo;

  configTime(0, 0, "hora.roa.es");
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println("Hora configurada de hora.roa.es");

  String timezone = "CET-1CEST,M3.5.0/1,M10.5.0";
  setenv("TZ", timezone.c_str(), 1);
  tzset();

}

