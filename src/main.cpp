#include <Arduino.h>
#include "configuration.h"
#include "hmi.h"
#include "imu.h"
#include "wifi_lab.h"
#include "sleep_lab.h"
#include "sender_lab.h"
#include "ads.h"
#include "web_lab.h"

#define pin_tension 4


// Horario
int hora;


// Ticker remember; // envio cada 2 horas de un pitido para recordar el uso del cubo

int valor_cara = 99; //Inicializamos valor_cara a 99. Los valores válidos son entre 0 y 5.


void wakeup_2h()
{
  int wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER)
  {
    toneBuzz(0);
  }
  return;
}

void beep_time()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain the current time");
    return;
  }
  hora = timeinfo.tm_hour;
  Serial.println(hora);
  if (hora <= 21 && hora > 9) // rango de 9AM a 10 AM
  {
    wakeup_2h();
  }
  return;
}


void modoConfiguracion()
{

    Serial.println("WiFi desconectado");
    WiFi.disconnect(true);
    
    Serial.println("Creamos WifiCube");
    WiFi.softAP(ssidAP.c_str(), passwordAP.c_str());
    delay(100);


    Serial.println("Añadimos rutas de servidor");
    server.on("/", handleRoot);
    server.on("/select", handleSelect);
    server.on("/save", handleSave);
    server.on("/carga.gif", handleGif);
    server.on("/cubolab.jpg", handleImage);
    Serial.println("Iniciamos Servidor");
    server.begin();

    scanNetworks();

    Serial.println("Servidor Iniciado");
    Serial.println("Modo punto de acceso iniciado");
}


void setup()
{
  Serial.begin(115200);
 
  Serial.println("");
  Serial.println("Iniciando cubo");
  
  initLeds();
  
  Serial.println("Iniciando IMU");
  initIMU();

  Serial.println("Configurando IMU");
  configIMU();

  Serial.println("Configurando WiFi");
  connectWiFi();

  Serial.println("Configurando ADS");
  initADS();
  
  Serial.println("Configurando Memoria");
  initMemory();

  Serial.println("Configurando Gestor Archivos Servidor");
  configFileSystem();

  Serial.println("Configurando Buzzer");
  initBuzz();

  Serial.println("Configuraciones ESP32 Modo Bajo Consumo");
  configBatterySleep();

  
  Serial.println("Obtenemos hora");
  configTime();

  
  Serial.println("Activamos Buzzer recordatorio");
  //activarBuzzer();
  
  // Activamos pines de interrupciones
  pinMode(4, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);

  // Si no estamos en modo configuración permitimos interrupciones
  if (modoConfig == false)
  {
    mpu.setMotionInterrupt(true);
    mpu.setMotionDetectionThreshold(5.0f); // deteccion de un cambio de gravedad en un incremento de 5m/s^2
    mpu.setMotionDetectionDuration(2);
  }
  
  Serial.println ("Fin de la Configuración Inicial");

}

void loop()
{
  
  //if (digitalRead(pin_tension) == HIGH && !modoconfig) // entrar en modo config cuando se conecta al cargador
  if (modoConfig) // entrar en modo config 
  {
    if (firstConfig){
      Serial.println("Primera configuración");
      modoConfiguracion();
      Serial.println("Termina primera configuracion");
      firstConfig = false;
    }
    server.handleClient();
  }

  // Modo funcionamiento NORMAL
  if (WiFi.status() == WL_CONNECTED && !modoConfig)
  {
    Serial.println("Funcionando en modo normal");
    delay(3000);

    //Muestra con leds la carga de la batería
    // battery();

    readIMU();

    calculateSide();

    sendData();
    
    goToSleep();

  }

}