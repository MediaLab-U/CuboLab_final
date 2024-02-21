#include "wifi_lab.h"

String networks = "";
boolean networksAvailable = false;

String ssid;
String password;

String ssidAP = "ConfigCUBO";
String passwordAP = "cubolab00";

char macStr[18];

void getMac(){
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  mac[0] = 0;
}

boolean connectWiFi() {
  // Obtenemos mac dispositivo
  getMac();
  
  // Conexión a la red WiFi
  ssid = preferences.getString("ssid", "MikroTik-B87EBD");
  password = preferences.getString("pass", "medialab2019");

  Serial.println(ssid.c_str());
  Serial.println(password.c_str());

  WiFi.begin(ssid.c_str(), password.c_str());
  delay(1000);

  int attempts  = 0;
  while (attempts  < 10) {
    WiFi.begin(ssid.c_str(), password.c_str());
    delay(1000); // Espera un momento para la conexión

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Conectado a la red WiFi");
      Serial.print("Dirección IP: ");
      Serial.println(WiFi.localIP());
      break; // Sal del bucle si la conexión tiene éxito
    } else {
      attempts ++;
      Serial.println("Conexión fallida. Intento nuevamente...");
    }
  }

  if (attempts  >= 9) {
    // ToDo: Realiza alguna acción si no se puede conectar después de 10 intentos
    Serial.println("No se pudo conectar a la red WiFi después de 10 intentos.");
    return false;
  }

  return true;

}


void scanNetworks()
{
  networksAvailable = false;

  Serial.println("Detectando WiFis cercanas");



  int nNetworks = WiFi.scanNetworks();

  for (int i = 0; i < nNetworks; i++)
  {
    String ssidd = WiFi.SSID(i);
    networks += "<option value='" + ssidd + "'>" + ssidd + "</option>";

  }

  networksAvailable = true;
}