#include "web_lab.h"

WebServer server(80);
boolean wifiUpdate = false;

WiFiMulti wifiMulti;

void handleRoot()
{
    // Si el escaner de redes wifi está lista devolemos el html con las wifis
    if (networksAvailable){
        // Lee el contenido del archivo HTML
        String content;
        File file = LittleFS.open("/index.html", "r");
        if (file) {
            while (file.available()) {
                content += file.readStringUntil('\n');
            }
            file.close();
        } else {
            Serial.println("No se pudo abrir el archivo content.html");
            return;
        }

        // Concatena datos de variables de Arduino en el HTML
        content.replace("{networks}", networks);

        if(wifiUpdate){
          content.replace("{WifiUpdate}", "<div id='banner' style='background-color: #4caf50; color: white; padding: 10px; text-align: center;'><strong>¡Nueva WiFi guardada correctamente!</strong></div>");
        }else{
          content.replace("{WifiUpdate}", "");
        }

        String wifiSsid="ssid1";
        String ssid1 = preferences.getString(wifiSsid.c_str(), "");
        wifiSsid="ssid2";
        String ssid2 = preferences.getString(wifiSsid.c_str(), "");
        wifiSsid="ssid3";
        String ssid3 = preferences.getString(wifiSsid.c_str(), "");
        wifiSsid="ssid4";
        String ssid4 = preferences.getString(wifiSsid.c_str(), "");
        wifiSsid="ssid5";
        String ssid5 = preferences.getString(wifiSsid.c_str(), "");
      
        content.replace("{MAC}", macStr);
        content.replace("{batteryVoltage}", String(readBatteryLevel()));
        content.replace("{batteryPorcentage}", String(readBatteryPorcentage()));
        content.replace("{ssid1}", ssid1);
        content.replace("{ssid2}", ssid2);
        content.replace("{ssid3}", ssid3);
        content.replace("{ssid4}", ssid4);
        content.replace("{ssid5}", ssid5);

        server.send(200, "text/html", content);

    }
    // Si todavía no se completo devolvemos página de carga con tasa de refresco de 10s
    else{
        server.sendHeader("Refresh", "10");
        server.send(200, "text/html", "<html><body><h1>Cargando...</h1><img src='/carga.gif'></body></html>");

    }    
    

}



void handleSave()
{
  ssid = server.arg("ssid");
  password = server.arg("password");

  int counter = preferences.getInt("counter", 0);
  
  // Si el contador llega a 5, reiniciar a 1
  if (counter >= 5) {
    counter = 1;
  } else {
    counter++; // Incrementar el contador
  }

  // Crear las claves para el ssid y el password basadas en el contador
  String ssidKey = "ssid" + String(counter);
  String passKey = "pass" + String(counter);


  // Guardar el ssid y el password utilizando las claves correspondientes
  preferences.putString(ssidKey.c_str(), ssid);
  preferences.putString(passKey.c_str(), password);

 // Convertir los SSID y contraseñas a const char*
  const char* ssidChar = ssid.c_str();
  const char* passwordChar = password.c_str();

  // Agregar la red WiFi con su contraseña
  wifiMulti.addAP(ssidChar, passwordChar);
  
  // Guardar el nuevo valor del contador
  preferences.putInt("counter", counter);

  wifiUpdate = true;

  handleRoot();
  

}

void handleTest()
{

  State state2 = static_cast<State>(server.arg("state").toInt());
  
  handleState(state2);

  handleRoot();
  

}

void handleExit()
{
  // Lee el contenido del archivo HTML
  String content;
  File file = LittleFS.open("/out.html", "r");
  if (file) {
      while (file.available()) {
          content += file.readStringUntil('\n');
      }
      file.close();
  } else {
      Serial.println("No se pudo abrir el archivo out.html");
      return;
  }


  server.send(200, "text/html", content);
  long timeExit = millis();
  while((millis()-timeExit)<=(5000)){

  }

  WiFi.disconnect(true);

  ssid = preferences.getString("ssid", "MikroTik-B87EBD");
  password = preferences.getString("pass", "medialab2019");
  
  if(!connectWiFi()){
    State state = NO_CONNECTION;
    handleState(state);

    goToSleep();
  }

  cubeState = CHARGE;
  firstConfig = true;


}


void handleGif()
{
  File imageFile1 = LittleFS.open("/carga.gif", "r");
  if (imageFile1)
  {
    server.streamFile(imageFile1, "image/gif");
    imageFile1.close();
  }
}

void handleImage()
{
  File imageFile = LittleFS.open("/cubolab.jpg", "r");
  if (imageFile)
  {
    server.streamFile(imageFile, "image/jpg");
    imageFile.close();
  }
}

