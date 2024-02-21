#include "configuration.h"
CubeState cubeState = NORMAL_MODE;
boolean firstConfig = true;
Preferences preferences;
String cubeTime;


void initMemory() {
  preferences.begin("MediaLab", false);
  ssid = preferences.getString("ssid", "MikroTik-B87EBD");
  password = preferences.getString("pass", "medialab2019");
}

void configFileSystem() {
  LittleFS.begin();
}



void configMode()
{
    getMac();

    Serial.println("WiFi desconectado");
    WiFi.disconnect(true);
    
    Serial.println("Creamos WifiCube");
    WiFi.softAP(ssidAP.c_str(), passwordAP.c_str());
    delay(100);

    Serial.println("Añadimos rutas de servidor");
    server.on("/", handleRoot);
    server.on("/save", handleSave);
    server.on("/exit", handleExit);
    server.on("/image", handleImage);
    Serial.println("Iniciamos Servidor");
    server.begin();

    scanNetworks();

    Serial.println("Servidor Iniciado");
    Serial.println("Modo punto de acceso iniciado");
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

boolean getTime() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return false;
  }

  char formatTime[6]; // hh:mm\0
  strftime(formatTime, sizeof(formatTime), "%H:%M", &timeinfo);

  cubeTime = String(formatTime);
  return true;
}

boolean isDay(){

  int h = cubeTime.substring(0, 2).toInt();
  
  // Comprobar si la hora está entre las 8:00 y las 22:00
  if (h >= 8 && h <= 21) {
    return true;
  } else {
    return false;
  }
}

void delayLab(long wait){
  long tt =millis();
  while((millis()-tt)<wait){}
}

