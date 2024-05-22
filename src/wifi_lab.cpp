#include "wifi_lab.h"

String networks = "";
boolean networksAvailable = false;

String ssid;
String password;

String ssidAP = "ConfigCuboLab";
String passwordAP = "cubolab00";


char macStr[18];

void getMac(){
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  mac[0] = 0;
}

String getLastMac(){
  byte mac[6];
  WiFi.macAddress(mac);
  char cMac[18] = {0};
  sprintf(cMac, "%02X-%02X-%02X",  mac[3], mac[4], mac[5]);
  return String(cMac);
}

boolean connectWiFi() {

  getMac();
  Serial.println(macStr);

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

boolean connectWiFi2(){
  Serial.println("MultiWIFI");

  for (int i = 1; i <= 5; i++) {
    // Construir las claves de búsqueda para el ssid y el password
    String ssidKey = "ssid" + String(i);
    String passKey = "pass" + String(i);

    // Obtener el ssid y el password correspondientes al índice actual
    String ssid = preferences.getString(ssidKey.c_str(), "");
    String password = preferences.getString(passKey.c_str(), "");

    // Si no se encontraron ssid y password para esta clave, continuar con el siguiente índice
    if (ssid == "" || password == "") {
      continue;
    }

    // Convertir los SSID y contraseñas a const char*
    const char* ssidChar = ssid.c_str();
    const char* passwordChar = password.c_str();

    // Agregar la red WiFi con su contraseña
    wifiMulti.addAP(ssidChar, passwordChar);
  }

  int attempts  = 0;
  while (attempts  < 10) {
    delay(1000); // Espera un momento para la conexión

    if (wifiMulti.run() == WL_CONNECTED) {
      Serial.println("Conectado a la red WiFi");
      Serial.print(WiFi.SSID());
      Serial.print(" ");
      Serial.println(WiFi.RSSI());
      Serial.print("Dirección IP: ");
      Serial.println(WiFi.localIP());
      break; // Sal del bucle si la conexión tiene éxito
    } 
    
    else {
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



void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void updateFirmware(){
  Serial.println("Update Firmware");
  WiFiClient client;

  // The line below is optional. It can be used to blink the LED on the board during flashing
  // The LED will be on during download of one buffer of data from the network. The LED will
  // be off during writing that buffer to flash
  // On a good connection the LED should flash regularly. On a bad connection the LED will be
  // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
  // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
  
  httpUpdate.setLedPin(led_r, HIGH);

  httpUpdate.onStart(update_started);
  httpUpdate.onEnd(update_finished);
  httpUpdate.onProgress(update_progress);
  httpUpdate.onError(update_error);

  t_httpUpdate_return ret = httpUpdate.update(client, "http://85.31.236.104:3035/update?version=1.1");
  // Or:
  //t_httpUpdate_return ret = httpUpdate.update(client, "server", 80, "/file.bin");

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");

      handleState(UPDATED_OK);  
      ESP.restart();
      break;
  }

}