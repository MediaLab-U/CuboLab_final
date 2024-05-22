#include "configuration.h"
CubeState cubeState = NORMAL_MODE;
boolean firstConfig = true;
Preferences preferences;
String cubeTime;
long long int cuboSleep = 7200000000LL;


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
    String apName = "ConfigCubo-";
    
    apName += getLastMac();

    Serial.println(apName);
    WiFi.softAP(apName, passwordAP.c_str());
    delay(100);

    Serial.println("Añadimos rutas de servidor");
    server.on("/", handleRoot);
    server.on("/save", handleSave);
    server.on("/test", handleTest);
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

  configTime();                                                               // Configuramos hora


  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return false;
  }

  char formatTime[6]; // hh:mm\0
  strftime(formatTime, sizeof(formatTime), "%H:%M", &timeinfo);

  cubeTime = String(formatTime);

  Serial.println("Cubo time getTime:");
  Serial.println(cubeTime);

  preferences.putString("lastTime", cubeTime);

  return true;
}

void timeToSleep(){
  cuboSleep = 7200000000LL;

  getTime();

  // Obtener la hora y los minutos de inicio
  int startH = cubeTime.substring(0, 2).toInt();
  int startM = cubeTime.substring(3).toInt();

  String lastTime = preferences.getString("lastTime", "06:00");

  // Obtener la hora y los minutos de fin
  int endH = lastTime.substring(0, 2).toInt()+2;
  if(endH >=24){
    endH -= 24;
  }
  int endM = lastTime.substring(3).toInt();

  if (endH < startH || (endH == startH && endM <= startM)) {
    Serial.println("Estoy aqui");
    cuboSleep = 7200000000LL;
    } else {
      // Si la hora final es posterior a la hora inicial en el mismo día
      
      if (endM < startM) {
          endH--; // Restar 1 hora a endH
          endM += 60; // Sumar 60 minutos a endM
      }
      long long int diffH = (endH - startH) * 3600LL; // Utiliza LL para forzar el cálculo como long long int
      long long int diffM = (endM - startM) * 60LL; // Utiliza LL para forzar el cálculo como long long int
      long long int cuboSleep = (diffH + diffM) * 1000000LL; // Utiliza LL para forzar el cálculo como long long int
  }

  Serial.print("Cubo Time last: ");
  Serial.println(lastTime);

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

