#include <Arduino.h>
#include "configuration.h"

#define pin_tension 4

long t1;
  
State state;

float cubeVersion = 4.2;

float version;

void setup()
{

  /*------------------------------------------------------------ CONFIGURACIÓN INICIAL ------------------------------------------------------------*/

  Serial.begin(115200);
 
  Serial.println("");
  Serial.print("Iniciando cubo v");
  Serial.println(cubeVersion);
  
  Serial.println("Configurando Human Interface");             // Configurar Leds y Buzzer
  initHMI();

  boolean testHMI = false;

  Serial.print("CubeVersion: ");
  Serial.println(cubeVersion);

  if(testHMI) {
    Serial.println("ROJO");
    ledRed();
    delay(1000);
    ledsOff();
    delay(1000);
    Serial.println("VERDE");
    ledGreen();
    delay(1000);
    ledsOff();
    delay(1000);
    Serial.println("AZUL");
    ledBlue();
    delay(1000);
    ledsOff();
    delay(1000);

    testBuzzer();
  }

  Serial.println("Comprobamos nivel de bateria");
  state = readBatteryStateLab();                         // Leemos estado batería en carga
  handleState(state);                                      
  
  Serial.println("Iniciando IMU");                            // Inicialización y configuración del IMU
  initIMU();

  Serial.println("Configurando Memoria");                     // Inicialización de la memoria no voltail - !IMPORTANTE! tiene que estar antes de la configuración WiFi
  initMemory();

  if (version<cubeVersion){
    Serial.println("Nueva version");
    handleState(NEW_VERSION);
    preferences.putFloat("version", cubeVersion);
  }

  Serial.println("Configuraciones ESP32 Modo Bajo Consumo");  // Configurar parameros del esp durante sleep
  configSleep();
  
  Serial.println("Configurando Gestor Archivos Servidor");    // Inizialización de archivos para servidor (solo se usa en modo config)
  configFileSystem();


  
  /*--------------------------------------------------------------- POR QUÉ DESPIERTO --------------------------------------------------------------*/

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  Serial.print("Wake Up reason: ");
  Serial.println(wakeup_reason);
  switch(wakeup_reason) {

    case ESP_SLEEP_WAKEUP_EXT0:
      // CARGA
      if (analogRead(VCharge) > 2500) {
        Serial.println("El dispositivo se ha enchufado");
        cubeState = WIFI_CONFIG;
      } 
      // MOVIMIENTO
      else {
        Serial.println("El dispositivo ha detectado un movimiento");
        cubeState = NORMAL_MODE;
      }
      break;

    case ESP_SLEEP_WAKEUP_TIMER:
      // TIMER
      Serial.println("Temporizador de recordatorio");

      Serial.println("Conectando WiFi");         
      if(!connectWiFi()){                                        
        // TO-DO mostrar error 

        if (analogRead(VCharge) > 2500) {
        Serial.println("El dispositivo se ha enchufado");
        cubeState = WIFI_CONFIG;
        }
        else{
          goToSleep();
        } 
      }
      else{
        updateFirmware();
        sendBattery();
        if(getTime()){

          // ¿Es de día? 
          if (isDay()){
            handleState(REMEMBER);  

          }
          goToSleep();
          
        } 
      }
      break;

    default:
      // CARGA
      if (analogRead(VCharge) > 2500) {
        Serial.println("El dispositivo se ha enchufado");
        cubeState = WIFI_CONFIG;
      } 
      // MOVIMIENTO
      else {
        Serial.println("El dispositivo ha detectado un movimiento");
        cubeState = NORMAL_MODE;
      }
      break;
  }

  // Hay veces que a pesar de estar enchufado piensa que despierta por movimiento
  // por eso lo volvemos a forzar aquí en caso de que esté enchufado.
  
  if (analogRead(VCharge) > 2500) {
        Serial.println("El dispositivo se ha enchufado");
        cubeState = WIFI_CONFIG;
      } 
  
  Serial.println ("Fin de la Configuración Inicial");

}

void loop()
{

  switch (cubeState) {
    
    case WIFI_CONFIG: 
        
        handleState(CONFIG);                                           // Mostramos por hmi la carga


        if (firstConfig){                                                    // La primera vez configuramos el punto AP
          
          Serial.println("Primera configuración");
          
          t1 = millis();
          
          configMode();
          
          Serial.println("Termina primera configuracion");
          firstConfig = false;
        }
        
        server.handleClient();                                              // Respondemos peticiones servidor web configuración

        if ((millis()-t1)>=(2*60*1000)){                                    // TimeOut de 2 min para finalizar configuración
          Serial.println("Se sale del modo configuración wifi por timeout");

          WiFi.disconnect(true);
          cubeState = CHARGE;
        }

        if (analogRead(VCharge) < 2500){                                     // Si se desenchufa salimos del modo configuración                       
          Serial.println("Salimos de modo configuración por desenchufar");
          WiFi.disconnect(true);
          cubeState = NORMAL_MODE;
        }

        break;
      
    case CHARGE:

      handleState(GREEN_CHARGE);                                        // Mostramos por hmi la carga

      if (analogRead(VCharge) < 2500){
          Serial.println("Salimos de modo charge");
          cubeState = NORMAL_MODE;
        }
      break;

    case NORMAL_MODE:

      // Activamos configuraciones de interrupción del IMU
      readIMU();
      calculateSide();

      if(!connectWiFi2()){    
        if (!sameSide()){
          handleState(NO_CONNECTION);                              
          
        }
        goToSleep();
      }
      if (WiFi.status() == WL_CONNECTED){
        if (!sameSide()){
          sendData(); 
          updateFirmware();
        }
        else{
          updateFirmware();
          timeToSleep();
        }

      }
      // To-Do
      if (analogRead(VCharge) > 2500) {
        Serial.println("El dispositivo se ha enchufado");
        cubeState = WIFI_CONFIG;
      }
      else{
        goToSleep();
      }
      break;

    default: 
      cubeState = NORMAL_MODE;
      break;

  }

}