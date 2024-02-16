#include "web_lab.h"
#include "wifi_lab.h"
#include "imu.h"
#include "ads.h"

WebServer server(80);

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
        content.replace("{batteryVoltage}", String(readBattery()));
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

void handleSelect()
{
  ssid = server.arg("ssid");
  server.send(200, "text/html", "SSID seleccionado: " + ssid + "\nPor favor, ingresa la contraseña correspondiente: <form method='POST' action='/save'><input type='password' name='password'><input type='submit' value='Guardar'></form>");

  Serial.println("ssid guardado");
}

void handleSave()
{

  password = server.arg("password");
  server.send(200, "text/plain", "Wifi actualizado");
  
  WiFi.disconnect(true);
  
  // Guardamos datos en memoria no voltalil de la nueva wifi
  preferences.putString("ssid", ssid);
  preferences.putString("pass", password);
  
  connectWiFi();

  modoConfig = false;
  firstConfig = true;

  mpu.setMotionInterrupt(true);
  mpu.setMotionDetectionThreshold(5.0f); // deteccion de un cambio de gravedad en un incremento de 5m/s^2
  mpu.setMotionDetectionDuration(2);


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

