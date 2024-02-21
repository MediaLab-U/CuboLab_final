#include <Arduino.h>
#include "configuration.h"

#define pin_tension 4

long t1;
  

State state;

void setup()
{
  Serial.begin(115200);
 
  Serial.println("");
  Serial.println("Iniciando cubo");
  
  Serial.println("Configurando Human Interface");             // Configurar Leds y Buzzer
  initHMI();

  Serial.println("Configurando ADS");                         // Configurar ADC para lectura de batería
  initADS();

  Serial.println("Comprobamos nivel de bateria");
  // To-Do
  state = readBatteryStateLab();                        // Leemos estado batería en carga
  handleState(state);                                      
  
  Serial.println("Iniciando IMU");                            // Inicialización y configuración del IMU
  initIMU();

  Serial.println("Configurando Memoria");                     // Inicialización de la memoria no voltail - !IMPORTANTE! tiene que estar antes de la configuración WiFi
  initMemory();

  Serial.println("Configuraciones ESP32 Modo Bajo Consumo");  // Configurar parameros del esp durante sleep
  configSleep();
  
  Serial.println("Configurando Gestor Archivos Servidor");    // Inizialización de archivos para servidor (solo se usa en modo config)
  configFileSystem();

  Serial.println("Conectando WiFi");                          // Conexión a la WiFi guardada.
  if(!connectWiFi()){                                         // Si no nos conectamos a la WiFi activamos modo configuración
    // TO-DO mostrar error 
    // handleState(NO_CONNECTION);
    cubeState = CHARGE;
    mpu.setMotionInterrupt(false);
  }
  
  else{                                                       // Si se conecta ...                         
    Serial.println("Obtenemos hora");                       
    configTime();

    // TO-DO
    // Serial.println("Activamos Buzzer recordatorio");
    // activarBuzzer();

    // Activamos configuraciones de interrupción del IMU
    mpu.setMotionInterrupt(true);
    mpu.setMotionDetectionThreshold(3.0f);                  // deteccion de un cambio de gravedad en un incremento de 5m/s^2
    mpu.setMotionDetectionDuration(1);
  }
  
  Serial.println ("Fin de la Configuración Inicial");

}

void loop()
{

  // Ejecutamos máquina de estados
  switch (cubeState) {
    
    case WIFI_CONFIG: 
        if (firstConfig){                                                 // La primera vez configuramos el punto AP
          
          Serial.println("Primera configuración");
          
          t1 = millis();
          
          configMode();
          
          Serial.println("Termina primera configuracion");
          firstConfig = false;
        }
        
        server.handleClient();                                            // Respondemos peticiones servidor web configuración

        state = readBatteryStateLab(true);                          // Leemos estado batería en carga
        Serial.println(state);
        
        handleState(GREEN_CHARGE);                                        // Mostramos por hmi la carga

        // Comprobamos si el cargador esta conectado
        
        if (!digitalRead(pin_tension)){
          // To-Do
          // Se sale al modo normal
        }

        if ((millis()-t1)>=(2*60*1000)){
          Serial.println("Se sale del modo configuración wifi por timeout");

          WiFi.disconnect(true);
          cubeState = CHARGE;
        }

        break;
      
    case CHARGE:
      state = readBatteryStateLab(true);                          // Leemos estado batería en carga  
      Serial.println(state);
      handleState(state);                                        // Mostramos por hmi la carga

      if (!digitalRead(pin_tension)){
          cubeState = NORMAL_MODE;
          Serial.println("Salimos de modo charge");
          state = readBatteryStateLab();                        // Leemos estado batería en carga
          Serial.println(state);
          handleState(state); 
        }
      break;

    case NORMAL_MODE:
      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("Funcionando en modo normal");

        readIMU();

        calculateSide();

        sendData();
        

      }
      goToSleep();
      break;

    default: 
      cubeState = NORMAL_MODE;
      break;

  }

}