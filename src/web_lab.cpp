#include "web_lab.h"

WebServer server(80);
boolean wifiUpdate = false;

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

        
        content.replace("{MAC}", macStr);
        content.replace("{batteryVoltage}", String(readBatteryLevel()));
        content.replace("{batteryPorcentage}", String(readBatteryPorcentage()));
        content.replace("{side}", String(currentSide));
        content.replace("{ssid}", ssid);
        content.replace("{password}", password);

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

  preferences.putString("ssid", ssid);
  preferences.putString("pass", password);


  ssid = preferences.getString("ssid", "MikroTik-B87EBD");
  password = preferences.getString("pass", "medialab2019");
  preferences.end();
  

  Serial.println(ssid.c_str());
  Serial.println(password.c_str());

  Serial.println("wifiUpdate");
  Serial.println(wifiUpdate);
  wifiUpdate = true;
  Serial.println(wifiUpdate);

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

  mpu.setMotionInterrupt(true);
  mpu.setMotionDetectionThreshold(5.0f); // deteccion de un cambio de gravedad en un incremento de 5m/s^2
  mpu.setMotionDetectionDuration(2);

  if(!connectWiFi()){
    State state = CORRECT_CONNECTION;
    handleState(state);

    goToSleep();
  }

  cubeState = NORMAL_MODE;
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

