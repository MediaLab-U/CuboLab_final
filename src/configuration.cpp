#include "configuration.h"
CubeState cubeState = NORMAL_MODE;
boolean firstConfig = true;
Preferences preferences;


void initMemory() {
  preferences.begin("MediaLab", false);
}

void configFileSystem() {
  LittleFS.begin();
}



void configMode()
{

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

String getTime() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "Error obteniendo la hora";
  }

  char formatTime[6]; // hh:mm\0
  strftime(formatTime, sizeof(formatTime), "%H:%M", &timeinfo);

  return String(formatTime);
}

String getNextTime(){
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "Error obteniendo la hora";
  }

  // Obtener la hora actual
  int horaActual = timeinfo.tm_hour;

  // Si la hora actual es par, calcular tiempo hasta la siguiente hora par
  if (horaActual % 2 == 0) {
    int minutosRestantes = 60 - timeinfo.tm_min;
    return String(minutosRestantes) + " minutos hasta la siguiente hora par";
  }

  // Si la hora actual es impar, calcular tiempo hasta la siguiente hora par
  int horasRestantes = (horaActual % 2 == 1) ? 1 : 0; // Si es 23:xx, restar 1 hora, sino 0
  int minutosRestantes = (60 - timeinfo.tm_min) + (horasRestantes * 60);
  return String(minutosRestantes) + " minutos hasta la siguiente hora par";
  
}

void delayLab(long wait){
  long tt =millis();
  while((millis()-tt)<wait){}
}

