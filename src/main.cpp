#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <esp_sleep.h>
#include <Preferences.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "I2Cdev.h"
#include <WebServer.h>
#include "ticker.h"

<<<<<<< Updated upstream
//Pines LEDS y seguidor de tensión
#define led_g 26
#define led_b 25
#define led_r 27
#define analog_input 35
=======

>>>>>>> Stashed changes

// Pines y dirección I2C
#define SDA_PIN 21
#define SCL_PIN 22
<<<<<<< HEAD
// #define MPU6050_I2C_ADDRESS 0x68 // Suele ser 0x68 o 0x69


int voltaje;

//Variables de credenciales de la Wifi
const char *ssid = "Alvaro";
const char *password = "98765432";
=======
#define MPU6050_I2C_ADDRESS 0x68 //Suele ser 0x68 o 0x69


int voltaje;
const char *ssid = "MediaLab guest";
const char *password = "medialab2019";
>>>>>>> 449557aaefb079cd31ade88acfa16c046772780d

HTTPClient http;

Preferences preferences;

char macStr[18];
byte mac[6];

Adafruit_MPU6050 mpu;
int valor_cara = 0;
int x = 0;
double timeini, timefin;
bool trabajoRealizado = false;
String actual;

<<<<<<< Updated upstream

void timerCallback()
=======
void ledAzul()
>>>>>>> Stashed changes
{
    // Código a ejecutar cuando se alcance el tiempo deseado
  //     WiFi.disconnect(true);
  // WiFi.begin(cuboSSID.c_str(), cuboPassword.c_str());
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }

  WiFi.disconnect(true);
  WiFi.begin(cuboSSID.c_str(), cuboPassword.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

<<<<<<< Updated upstream
=======
void beep_buzzer(int modo)
{
  switch (modo)
  {
  case 0:
    if (buzzer_flag == true)
    {
      analogWrite(GPIO_NUM_18, 255);
      delay(100);
      analogWrite(GPIO_NUM_18, 0);
      delay(100);
      analogWrite(GPIO_NUM_18, 255);
      delay(100);
      analogWrite(GPIO_NUM_18, 0);
      buzzer_flag = false;
    }
    break;
  case 1:
    analogWrite(GPIO_NUM_18, 80);
    delay(100);
    analogWrite(GPIO_NUM_18, 0);
    delay(100);
    analogWrite(GPIO_NUM_18, 80);
    delay(100);
    analogWrite(GPIO_NUM_18, 0);
    delay(100);
    analogWrite(GPIO_NUM_18, 80);
    delay(100);
    analogWrite(GPIO_NUM_18, 0);
    break;
  case 2:
    // tone(BUZZER_PIN, 800, 100);
    analogWrite(GPIO_NUM_18, 80);
    delay(100);
    analogWrite(GPIO_NUM_18, 0);
    // tone(BUZZER_PIN, 0, 100);
    break;
  }
}

void wakeup_2h()
{
  int wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER)
  {
    beep_buzzer(beep_2h);
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

// Cierre Hora ESP32
void cierreconfig()
{
  // Código a ejecutar cuando se alcance el tiempo deseado
  if (modoconfig == true)
  {
    WiFi.disconnect(true);
    String ssid1 = preferences.getString("ssid", "medialab");
    String password1 = preferences.getString("pass", "medialab");
    WiFi.begin(ssid1.c_str(), password1.c_str());
    Serial.println("No cambios config. Conectando wifi anterior..." + ssid1 + password1);
    modoconfig = false;
  }
>>>>>>> Stashed changes
  Serial.println("temporizador terminado");
   timer.detach();
}

void handleRoot() 
{

 String listaredes = "";
  int numredes = WiFi.scanNetworks();

  for (int i = 0; i < numredes; i++) {
    String ssidd = WiFi.SSID(i);
    Serial.print(".");
    listaredes += "<li><a href='/select?ssid=" + ssidd + "'>" + ssidd + "</a></li>";
  }

  String html = "<html><body>"
                //"<img src='/sensor01.jpg'>"
                "<h1>Redes WiFi disponibles:</h1>"
                "<ul>" + listaredes + "</ul>"
                "</body></html>";

  server.send(200, "text/html", html);
}

<<<<<<< Updated upstream

void handleSelect() {
=======
void handleRoot()
{
  if (scanningComplete)
  {
    // Redireccionar a la lista de redes
    String html = "<html><body>"
                  "<img src='/cubolab.jpg' width='100' height='100'>"
                  "<h1>Redes WiFi disponibles:</h1>"
                  "<ul>" +
                  listaredes + "</ul>"
                               "</body></html>";

    server.send(200, "text/html", html);
  }
  else
  {
    // Enviar la página de carga
    server.sendHeader("Refresh", "10");
    server.send(200, "text/html", "<html><body><h1>Cargando...</h1><img src='/carga.gif'></body></html>");

    delay(2000); // Agregar un retraso de 1 segundo

    escanredes(); // ejecutar función escanear redes
  }
}

void handleSelect()
{
>>>>>>> Stashed changes
  cuboSSID = server.arg("ssid");
  server.send(200, "text/html", "SSID seleccionado: " + cuboSSID + "\nPor favor, ingresa la contraseña correspondiente: <form method='POST' action='/save'><input type='password' name='password'><input type='submit' value='Guardar'></form>");
  //   String html = "<html><body>"
  //               "<h1>Redes WiFi disponibles:</h1>"
  //               "<ul>" + listaredes + "</ul>"
  //               "</body></html>";

  // server.send(200, "text/html", html);
  Serial.println("ssid guardado");
}

void handleSave() {
  cuboPassword = server.arg("password");
  server.send(200, "text/plain", "SSID y contraseña guardados: " + cuboSSID + ", " + cuboPassword);
  delay(2000);

  WiFi.disconnect(true);
  WiFi.begin(cuboSSID.c_str(), cuboPassword.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  preferences.putString("ssid", cuboSSID);
  preferences.putString("pass", cuboPassword);

  Serial.println("");
  Serial.println("Conexión establecida");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void IRAM_ATTR handleInterrupt()
{
  Serial.println("MOVIMIENTO DETECTADO!!!");
}

// void IRAM_ATTR cargando()
// {
//   Serial.println("Conectado a bateria");
// }

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  mpu.begin();
  preferences.begin("myPreferences", false);

<<<<<<< Updated upstream
  // Comenzar conexión I2C
  // Wire.begin(SDA_PIN, SCL_PIN);

  // Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  // Wire.write(0x6B); // Dirección del registro de configuración del MPU6050

  // Conexión a la red WiFi
  ssid1 = preferences.getString("ssid", "medialab");
  password1 = preferences.getString("pass", "medialab");
  WiFi.begin(ssid1.c_str(), password1.c_str());
=======
  
  /*ledGreen();
  delay(1000);
  ledsOff();
  delay(1000);

  ledBlue();
  delay(1000);
  ledsOff();
  delay(1000);
  
  ledYellow();
  delay(1000);
  ledsOff();
  delay(1000);
  
  ledRed();
  delay(1000);
  ledsOff();
  delay(1000);
  
  ledPurple();
  delay(1000);
  ledsOff();
  delay(1000);*/

  // testBuzzer();
  
  Serial.println("Comprobamos nivel de bateria");
  state = readBatteryStateLab(false);                         // Leemos estado batería en carga
  handleState(state);                                      
  
  Serial.println("Iniciando IMU");                            // Inicialización y configuración del IMU
  initIMU();
>>>>>>> Stashed changes

  // Configuración para dejar activos los pines de batería
  // Configurar los pines 4, 5 y 6 como entrada
  gpio_set_direction(GPIO_NUM_25, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_26, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_27, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_14, GPIO_MODE_INPUT);
  // Habilitar la retención de pines durante el modo de bajo consumo
  gpio_deep_sleep_hold_en();

  // Configurar el modo de bajo consumo para reducir el consumo de energía
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO);

  // Configuración de los pines para los leds
  pinMode(led_g, OUTPUT);
  pinMode(led_b, OUTPUT);
  pinMode(led_r, OUTPUT);
  digitalWrite(led_g, HIGH);
  digitalWrite(led_r, HIGH);
  digitalWrite(led_b, HIGH);

  pinMode(analog_input, INPUT);

  // Conexion a la red Wifi
  /****************************/
  Serial.print("Conectando a la red WiFi.");
  delay(100);
  Serial.print(".");
  delay(100);
  Serial.println(".");

  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(led_b, LOW);
    digitalWrite(led_g, HIGH);
    digitalWrite(led_r, HIGH);
    delay(800);
    digitalWrite(led_b, HIGH);
    digitalWrite(led_g, HIGH);
    digitalWrite(led_r, HIGH);
    delay(800);

<<<<<<< Updated upstream
    if (WiFi.status() == WL_CONNECTED)
    {
=======
    case ESP_SLEEP_WAKEUP_EXT0:
      // CARGA
      if (analogRead(33) > 2024) {
        Serial.println("El dispositivo se ha enchufado");
        cubeState = WIFI_CONFIG;
      } 
      // MOVIMIENTO
      else {
        Serial.println("El dispositivo ha detectado un movimiento");
        cubeState = NORMAL_MODE;
      }
>>>>>>> Stashed changes
      break;
    }

    x = x + 1;
    Serial.print(x);
    if (x == 20)
    {
      digitalWrite(led_g, HIGH);
      digitalWrite(led_r, HIGH);
      digitalWrite(led_b, HIGH);
      Serial.println("No se ha podido conectar a la red WIFI");
      // Serial.print("Entrando en modo AP...");
      // /////
      WiFi.disconnect(true);
      WiFi.softAP(ssid, password);
      delay(100);

      server.on("/", handleRoot);
      server.on("/select", handleSelect);
      server.on("/save", handleSave);

      server.begin();

      Serial.println("Modo punto de acceso iniciado");
      break;
      //esp_restart();
    }
  }

  digitalWrite(led_g, HIGH);
  digitalWrite(led_r, HIGH);
  digitalWrite(led_b, HIGH);

  //Serial.print("Entrando en modo AP...");
      /////
      // WiFi.disconnect(true);
      // WiFi.softAP(ssid, password);
      // delay(100);

      // server.on("/", handleRoot);
      // server.on("/select", handleSelect);
      // server.on("/save", handleSave);

      // server.begin();

      // Serial.println("Modo punto de acceso iniciado");

  if (WiFi.status() == WL_CONNECTED)
  {
  Serial.println("Conexión exitosa");
  /****************************/
  // Cierre conexion a la Wifi

  // Obtención de la dirección MAC del ESP32
  /****************************/
  WiFi.macAddress(mac);

  // Conversión de la dirección MAC a una cadena de caracteres
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  mac[0] = 0;

  while (!Serial)
    delay(1000); // will pause Zero, Leonardo, etc until serial console opens
  /****************************/
  // Cierre

  // Valoración del inicio del MPU6050
  /****************************/
  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
    {
      delay(1000);
      Serial.println("Reiniciando ESP32");
      esp_restart();
    }
  }
  Serial.println("MPU6050 Found!");
  /****************************/
  // Cierre

<<<<<<< Updated upstream
  // Calibración del MPU6050
  /****************************/
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.println("seleccionado rango");
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange())
=======
void inicializarADS() {
  if (!ads.begin())
    {
      Serial.println("Fallo del analógico-digital");
    }
}

void configurarMemoriaNoVolatil() {
  //Recoge datos en memoria no volátil
  preferences.begin("myPreferences", false);
}

void configurarFileSystem() {
  // sistema de archivos para imagenes de la pagina de config
  LittleFS.begin();
}

void configurarBuzzer() {
  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
}

void conectarWiFi() {
  // Conexión a la red WiFi
  String ssid1 = preferences.getString("ssid", "medialab");
  String password1 = preferences.getString("pass", "medialab");
  WiFi.begin(ssid1.c_str(), password1.c_str());
}

void configurarPinesBateria() {
    // Configuración para dejar activos los pines de batería
  // Configurar los pines 4, 5 y 6 como entrada
  //Estas cuatro líneas, creemos que no funcionan
  gpio_set_direction(GPIO_NUM_25, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_26, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_27, GPIO_MODE_INPUT);
  gpio_set_direction(GPIO_NUM_14, GPIO_MODE_INPUT);
}

void configurarModoBajoConsumo() 
{
  // Habilitar la retención de pines durante el modo de bajo consumo
  gpio_deep_sleep_hold_en();
    // Configurar el modo de bajo consumo para reducir el consumo de energía
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO);
}

void configurarPinesLeds() 
{
  // Configuración de los pines para los leds
  pinMode(led_g, OUTPUT);
  pinMode(led_b, OUTPUT);
  pinMode(led_r, OUTPUT);
  ledApagado();
}

void esperarConexionWiFi() 
{
  Serial.print("Conectando a la red WiFi.");

  unsigned long inicioConexion = millis();
  unsigned long tiempoEspera = 800;

  while (millis() - inicioConexion < NUM_INTENTOS_WIFI * 2 * tiempoEspera) 
>>>>>>> Stashed changes
  {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange())
  {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

<<<<<<< Updated upstream
  mpu.setFilterBandwidth(MPU6050_BAND_184_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth())
  {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
  /****************************/
  // Cierre de la calibración del MPU6050

  // Configuración del pin de interrupción del MPU6050
  /****************************/
  mpu.setMotionInterrupt(true);
  pinMode(14, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(14), handleInterrupt, RISING);
  /****************************/
  // Cierre de la configuración

  // Calibración de la sensibilidad del pin de interrpción
  /****************************/
  mpu.setMotionDetectionThreshold(1.0f); // deteccion de un cambio de gravedad en un incremento de 1m/s^2
  mpu.setMotionDetectionDuration(2);
  /****************************/
  // Cierre

  // Valor de tensión de la batería
  /****************************/
  pinMode(4, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(4), cargando, RISING);
=======
void configurarHora() {
    // Start time
    Serial.setDebugOutput(true);
    initTime("CET-1CEST,M3.5.0/1,M10.5.0");
}

void activarBuzzer() {
    // Buzzer wakeup
    beep_time();
>>>>>>> Stashed changes
}

double Ctimer(void)
{
  struct timeval tm;

  gettimeofday(&tm, NULL);

  return tm.tv_sec + tm.tv_usec / 1.0E6;
}

<<<<<<< Updated upstream
void loop()
{
  // Serial.println(macStr);
  //  Código indicador de batería//
  /*****************************/
  if (WiFi.status() == WL_CONNECTED)
=======
void battery()
{
  adc0 = ((ads.readADC_SingleEnded(0) - 630) * 100) / (948 - 630); // leemos el valor analógico presente en el pin
  if (adc0 >= 100)
  {
    adc0 = 100;
  }
  else if (adc0 <= 0)
  {
    adc0 = 0;
  }
  switch (adc0)
  {
  case 0 ... 9:
    ledRojo();
    beep_buzzer(beep_battery);

  case 10 ... 20:
    ledRojo();
    break;
  case 21 ... 70:
    ledAmarillo();   //27.10.23 Cambiado por Ramón
    break;
  default:
    ledVerde();
    break;
  }
  Serial.println(adc0);
  delay(1000);
}

void bateria(int *porcentaje)
{
  int porcent = (((ads.readADC_SingleEnded(0) - 730) * 100) / (948 - 730)); // leemos el valor analógico presente en el pin
  if (porcent >= 100)
    *porcentaje = 100;
  else if (porcent <= 0)
    *porcentaje = 0;
  else
    *porcentaje = porcent;
}

void modoConfiguracion()
{
    ledNaranja();
    WiFi.disconnect(true);
    WiFi.softAP(ssid, password);
    delay(100);

    server.on("/", handleRoot);
    server.on("/select", handleSelect);
    server.on("/save", handleSave);
    server.on("/carga.gif", handleGif);
    server.on("/cubolab.jpg", handleImage);
    server.begin();
    modoconfig = true;                 // variable que evalua el estado de config
    iniconfig = true;                  // variable para entrar una sola vez en modo config al ser conectado
    timer.attach(120.0, cierreconfig); // tiempo en segundos, al terminar ejecutará la funcion cierreconfig
    Serial.println("Modo punto de acceso iniciado");
}

void leerCaraYGuardarValores()
{
    // Código para leer la cara que está hacia arriba y guardar los valores
    /*************************/
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    //------------EJE X-------------------//
    if (abs((int)a.acceleration.x) > SENSIBILIDAD_SENSOR)
    {
      timeini = Ctimer();
      if ((int)a.acceleration.x > 0 && valor_cara != 1)
      {
        valor_cara = 1;
        while (1)
        {
          mpu.getEvent(&a, &g, &temp);
          int x = (int)a.acceleration.x;
          delay(1000);
          if (x < 0 || abs(x) < SENSIBILIDAD_SENSOR || (Ctimer() - timeini) > 3)
          {
            break;
          }
        }
        timefin = Ctimer() - timeini;
        Serial.println(timefin);
        if (timefin > 3)
        {
          timeini = 0;
          timefin = 0;
          actual = "Estoy muy feliz";
          Serial.println(actual);
          trabajoRealizado = true;
        }
      }
      else if ((int)a.acceleration.x < 0 && valor_cara != 6)
      {
        valor_cara = 6;
        while (1)
        {
          mpu.getEvent(&a, &g, &temp);
          int x = (int)a.acceleration.x;
          delay(1000);
          if (x > 0 || abs(x) < SENSIBILIDAD_SENSOR || (Ctimer() - timeini) > 3)
          {
            break;
          }
        }
        timefin = Ctimer() - timeini;
        Serial.println(timefin);
        if (timefin > 3)
        {
          timeini = 0;
          timefin = 0;
          actual = "Estoy muy mal";
          Serial.println(actual);
          trabajoRealizado = true;
        }
      }
    }

    //------------EJE Y-------------------//
    if (abs((int)a.acceleration.y) > SENSIBILIDAD_SENSOR)
    {
      timeini = Ctimer();
      if ((int)a.acceleration.y > 0 && valor_cara != 4)
      {
        valor_cara = 4;
        while (1)
        {
          mpu.getEvent(&a, &g, &temp);
          int y = (int)a.acceleration.y;
          delay(1000);
          if (y < 0 || abs(y) < SENSIBILIDAD_SENSOR || (Ctimer() - timeini) > 3)
          {
            break;
          }
        }
        timefin = Ctimer() - timeini;
        Serial.println(timefin);
        if (timefin > 3)
        {
          timeini = 0;
          timefin = 0;
          actual = "Estoy mal";
          Serial.println(actual);
          trabajoRealizado = true;
        }
      }
      else if ((int)a.acceleration.y < 0 && valor_cara != 2)
      {
        valor_cara = 2;
        while (1)
        {
          mpu.getEvent(&a, &g, &temp);
          int y = (int)a.acceleration.y;
          delay(1000);
          if (y > 0 || abs(y) < SENSIBILIDAD_SENSOR || (Ctimer() - timeini) > 3)
          {
            break;
          }
        }
        timefin = Ctimer() - timeini;
        Serial.println(timefin);
        if (timefin > 3)
        {
          timeini = 0;
          timefin = 0;
          actual = "Estoy feliz";
          Serial.println(actual);
          trabajoRealizado = true;
        }
      }
    }

    //------------EJE Z-------------------//
    if (abs((int)a.acceleration.z) > SENSIBILIDAD_SENSOR)
    {
      Serial.println("Valor cara:" + valor_cara);
      timeini = Ctimer();
      if ((int)a.acceleration.z > 0 && valor_cara != 5)
      {
        valor_cara = 5;
        while (1)
        {
          mpu.getEvent(&a, &g, &temp);
          int z = (int)a.acceleration.z;
          delay(1000);
          if (z < 0 || abs(z) < SENSIBILIDAD_SENSOR || (Ctimer() - timeini) > 3)
          {
            break;
          }
        }
        timefin = Ctimer() - timeini;
        Serial.println(timefin);
        if (timefin > 3)
        {
          timeini = 0;
          timefin = 0;
          actual = "Estoy bastante mal";
          Serial.println(actual);
          trabajoRealizado = true;
        }
      }
      else if ((int)a.acceleration.z < 0 && valor_cara != 3)
      {
        valor_cara = 3;
        while (1)
        {
          mpu.getEvent(&a, &g, &temp);
          int z = (int)a.acceleration.z;
          delay(1000);
          if (z > 0 || abs(z) < SENSIBILIDAD_SENSOR || (Ctimer() - timeini) > 3)
          {
            break;
          }
        }
        timefin = Ctimer() - timeini;
        Serial.println(timefin);
        if (timefin > 3)
        {
          timeini = 0;
          timefin = 0;
          actual = "Estoy ni bien ni mal";
          Serial.println(actual);
          trabajoRealizado = true;
        }
      }
    }

}

// Despertar al ESP32 según las condiciones
void despertarSegunCondiciones()
{
    // // Despertar al ESP32 cuando se conecte a la red
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 1);

    // Apagar el ESP32 cuando no tenga suficiente batería
    if (adc0 <= 0 && digitalRead(GPIO_NUM_4) != 1)
    {
      Serial.println("No tengo tension");

      //Podríamos enviar un mensaje de alerta antes de ponerme a dormir
      esp_deep_sleep_start();
    }

    // Despertar al ESP32 cuando se mueva el MPU
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 1);
}

void enviarADormir()
{
      // Código para enviar a dormir el SP32 y despertarlo cada 15 minutos
      Serial.println("Me voy a dormir");

      // Se va a dormir 2 horas
      long long int TIME_TO_SLEEP = 7200000000LL; // 2h en us
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP);
      esp_deep_sleep_start();
}
    // Comprobación del trabajo realizado y envío de datos

void manejarTrabajoRealizado()
{
    // Parte encargada de comprobar si se ha realizado el trabajo, comprobar el valor anterior en memoria
    // y enviar los datos a la base de datos
    if (trabajoRealizado)
    {
      trabajoRealizado = false;
      String mensaje = preferences.getString("estadoAnterior", "Ninguno");
      Serial.println("El estado anterior es: " + mensaje + " y el actual: " + actual + " cara: " + valor_cara);
      if (mensaje != actual)
      {
        int porcentaje;
        bateria(&porcentaje);
        String mensajeHTTP = "https://www.unioviedo.es/medialab/datos_cube.php?e=" + String(valor_cara) + "&m='" + (String)macStr + "'&b=" + String(porcentaje);
        http.begin(mensajeHTTP);
        int httpCode = http.GET();
        Serial.println(mensajeHTTP);

        // Impresión de la respuesta en el monitor serie
        if (httpCode > 0)
        {
          String payload = http.getString();
          Serial.println(payload);
          // Led blanco
          ledBlanco();
          // Buzzer
          beep_buzzer(beep_send);
          // Cierre Buzzer
        }
        else
        {
          Serial.println("Error en la solicitud");
        }
      }

      preferences.putString("estadoAnterior", actual);
      Serial.println("El nuevo estado guardado en memoria no volátil es: " + preferences.getString("estadoAnterior", "Ninguno"));
      preferences.end();

      // Apagar el LED
      ledApagado();

      enviarADormir();
    }
}

    // Código para enviar a dormir el ESP32 y despertarlo cada 15 minutos

void setup()
{
  Serial.begin(115200);
 
  // Try to initialize!
 
  inicializarMPU();
  inicializarADS();
  configurarMemoriaNoVolatil();
  configurarFileSystem();
  configurarBuzzer();
  conectarWiFi();
  configurarPinesBateria();
  configurarModoBajoConsumo();
  configurarPinesLeds();
  esperarConexionWiFi();
  configurarMPU();
  configurarHora();
  activarBuzzer();

  if (modoconfig == false)
  {
    mpu.setMotionInterrupt(true);
    pinMode(14, INPUT_PULLUP);
    // attachInterrupt(digitalPinToInterrupt(14), handleInterrupt, RISING);
    mpu.setMotionDetectionThreshold(1.0f); // deteccion de un cambio de gravedad en un incremento de 1m/s^2
    mpu.setMotionDetectionDuration(2);

    pinMode(4, INPUT_PULLUP);
  }
  
  Serial.println ("fin del setup");

}

void loop()
{
  server.handleClient();

  if (digitalRead(pin_tension) == HIGH && !modoconfig && !iniconfig) // entrar en modo config cuando se conecta al cargador
>>>>>>> Stashed changes
  {
  int valor_actual = analogRead(analog_input); // leemos el valor analógico presente en el pin
  float v_real = (valor_actual * (5.00 / 1023.00)) * 2.8;
  Serial.println(digitalRead(GPIO_NUM_4));
  Serial.println(v_real);

  // while (pin_tension == HIGH) 
  // {
  //   WiFi.disconnect(true);
  //   WiFi.softAP(ssid, password);
  //   delay(10);

  //   server.on("/", handleRoot);
  //   server.on("/select", handleSelect);
  //   server.on("/save", handleSave);

  //   server.begin();

  //   Serial.println("Modo punto de acceso iniciado");

  //   timer.attach(30.0, timerCallback);

  // }

  if (v_real < 12 && v_real >= 10)
  {
    digitalWrite(led_r, LOW);
    digitalWrite(led_g, HIGH);
    digitalWrite(led_b, HIGH);
  }
<<<<<<< Updated upstream
  else if (v_real >= 12 && v_real < 14)
=======
  else if (digitalRead(pin_tension) == LOW && iniconfig)
>>>>>>> Stashed changes
  {
    digitalWrite(led_r, HIGH);
    digitalWrite(led_g, LOW);
    digitalWrite(led_b, LOW);
  }
  else if (v_real >= 14)
  {
    digitalWrite(led_r, HIGH);
    digitalWrite(led_g, LOW);
    digitalWrite(led_b, HIGH);
  }
  /***********************/
  // Cierre código indicador de batería

  preferences.begin("myPreferences", true); // Sentencia para guardar valores en memoria no volatil

  // Código para leer la cara que está hacia arriba y guardar los valores
  /*************************/
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  //------------EJE X-------------------//
  if (abs((int)a.acceleration.x) > 8)
  {
    timeini = Ctimer();
    if ((int)a.acceleration.x > 0 && valor_cara != 1)
    {
      valor_cara = 1;
      while (1)
      {
        mpu.getEvent(&a, &g, &temp);
        int x = (int)a.acceleration.x;
        delay(1000);
        if (x < 0 || abs(x) < 8 || (Ctimer() - timeini) > 3)
        {
          break;
        }
<<<<<<< Updated upstream
=======
        
        server.handleClient();                                            // Respondemos peticiones servidor web configuración

        if ((millis()-t1)>=(2*60*1000)){
          Serial.println("Se sale del modo configuración wifi por timeout");

          WiFi.disconnect(true);
          cubeState = CHARGE;
        }

        break;
      
    case CHARGE:
      state = readBatteryStateLab(true);                          // Leemos estado batería en carga  

      handleState(state);                                        // Mostramos por hmi la carga

      if (!digitalRead(VCHARGE)){
          Serial.println("Salimos de modo charge");
          cubeState = NORMAL_MODE;
        }
      break;

    case NORMAL_MODE:

      // Activamos configuraciones de interrupción del IMU
      readIMU();
      calculateSide();

      if(!connectWiFi()){    
        handleState(NO_CONNECTION);                              
        goToSleep();
>>>>>>> Stashed changes
      }
      timefin = Ctimer() - timeini;
      Serial.println(timefin);
      if (timefin > 3)
      {
        timeini = 0;
        timefin = 0;
        actual = "Estoy muy feliz";
        Serial.println(actual);
        trabajoRealizado = true;
      }
    }
    else if ((int)a.acceleration.x < 0 && valor_cara != 6)
    {
      valor_cara = 6;
      while (1)
      {
        mpu.getEvent(&a, &g, &temp);
        int x = (int)a.acceleration.x;
        delay(1000);
        if (x > 0 || abs(x) < 8 || (Ctimer() - timeini) > 3)
        {
          break;
        }
      }
      timefin = Ctimer() - timeini;
      Serial.println(timefin);
      if (timefin > 3)
      {
        timeini = 0;
        timefin = 0;
        actual = "Estoy muy mal";
        Serial.println(actual);
        trabajoRealizado = true;
      }
    }
  }

<<<<<<< Updated upstream
  //------------EJE Y-------------------//
  if (abs((int)a.acceleration.y) > 8)
  {
    timeini = Ctimer();
    if ((int)a.acceleration.y > 0 && valor_cara != 4)
    {
      valor_cara = 4;
      while (1)
      {
        mpu.getEvent(&a, &g, &temp);
        int y = (int)a.acceleration.y;
        delay(1000);
        if (y < 0 || abs(y) < 8 || (Ctimer() - timeini) > 3)
        {
          break;
        }
      }
      timefin = Ctimer() - timeini;
      Serial.println(timefin);
      if (timefin > 3)
      {
        timeini = 0;
        timefin = 0;
        actual = "Estoy mal";
        Serial.println(actual);
        trabajoRealizado = true;
      }
    }
    else if ((int)a.acceleration.y < 0 && valor_cara != 2)
    {
      valor_cara = 2;
      while (1)
      {
        mpu.getEvent(&a, &g, &temp);
        int y = (int)a.acceleration.y;
        delay(1000);
        if (y > 0 || abs(y) < 8 || (Ctimer() - timeini) > 3)
        {
          break;
        }
      }
      timefin = Ctimer() - timeini;
      Serial.println(timefin);
      if (timefin > 3)
      {
        timeini = 0;
        timefin = 0;
        actual = "Estoy feliz";
        Serial.println(actual);
        trabajoRealizado = true;
      }
    }
  }

  //------------EJE Z-------------------//
  if (abs((int)a.acceleration.z) > 8)
  {
    Serial.println("Valor cara:" + valor_cara);
    timeini = Ctimer();
    if ((int)a.acceleration.z > 0 && valor_cara != 5)
    {
      valor_cara = 5;
      while (1)
      {
        mpu.getEvent(&a, &g, &temp);
        int z = (int)a.acceleration.z;
        delay(1000);
        if (z < 0 || abs(z) < 8 || (Ctimer() - timeini) > 3)
        {
          break;
        }
      }
      timefin = Ctimer() - timeini;
      Serial.println(timefin);
      if (timefin > 3)
      {
        timeini = 0;
        timefin = 0;
        actual = "Estoy bastante mal";
        Serial.println(actual);
        trabajoRealizado = true;
      }
    }
    else if ((int)a.acceleration.z < 0 && valor_cara != 3)
    {
      valor_cara = 3;
      while (1)
      {
        mpu.getEvent(&a, &g, &temp);
        int z = (int)a.acceleration.z;
        delay(1000);
        if (z > 0 || abs(z) < 8 || (Ctimer() - timeini) > 3)
        {
          break;
        }
      }
      timefin = Ctimer() - timeini;
      Serial.println(timefin);
      if (timefin > 3)
      {
        timeini = 0;
        timefin = 0;
        actual = "Estoy ni bien ni mal";
        Serial.println(actual);
        trabajoRealizado = true;
      }
    }
  }
  /***********************/

  // Deep Sleep when the battery hasn't enought voltage
  /****************************/
  if (v_real <= 11 && digitalRead(GPIO_NUM_4) != 1)
  {
    Serial.println("No tengo tension");
    esp_deep_sleep_start();
  }

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 1);

  while (digitalRead(GPIO_NUM_4) == 1)
  {
    int valor_actual1 = analogRead(analog_input); // leemos el valor analógico presente en el pin
    float v_real1 = (valor_actual1 * (5.00 / 1023.00)) * 2.8;
    analogWrite(led_r, 150);
    analogWrite(led_g, 30);
    analogWrite(led_b, HIGH);
    if (v_real1 >= 14)
    {
      digitalWrite(led_r, HIGH);
      digitalWrite(led_g, LOW);
      digitalWrite(led_b, HIGH);
      break;
    }
  }

  // ESP32 wakeup when the MPU moves
  /****************************/
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 1);
  /****************************/

  // Serial.println(mpu.getMotionInterruptStatus());
  // Parte encargada de comprobar si se ha realizado el trabajo, comprobar el valor anterior en memoria
  // y enviar los datos a la base de datos
  if (trabajoRealizado)
  {
    trabajoRealizado = false;
    String mensaje = preferences.getString("estadoAnterior", "Ninguno");
    Serial.println("El estado anterior es: " + mensaje + " y el actual: " + actual + " cara: " + valor_cara);
    if (mensaje != actual)
    {
      digitalWrite(led_r, HIGH);
      digitalWrite(led_g, LOW);
      digitalWrite(led_b, HIGH);
      String mensajeHTTP = "https://www.medialab-uniovi.es/iot/datos_cube.php?m='" + (String)macStr + "&e=" + valor_cara + "&b=" + 50;
      http.begin(mensajeHTTP);
      int httpCode = http.GET();
      Serial.println(mensajeHTTP);
      // Impresión de la respuesta en el monitor serie
      if (httpCode > 0)
      {
        String payload = http.getString();
        Serial.println(payload);
      }
      else
      {
        Serial.println("Error en la solicitud");
      }
    }
    preferences.putString("estadoAnterior", actual);
    Serial.println("El nuevo estado guardado en memoria no volátil es: " + preferences.getString("estadoAnterior", "Ninguno"));
    preferences.end();

    // Apagar el LED
    digitalWrite(led_g, HIGH);
    digitalWrite(led_r, HIGH);
    digitalWrite(led_b, HIGH);
    // Código para enviar a dormi el SP32 y despertarlo cada 15 minutos
    Serial.println("Me voy a dormir");

    esp_deep_sleep_start();
  }
  // esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 1);

  // if (v_real <= 11 && GPIO_NUM_4 != 1)
  // {
  //   Serial.println("No tengo tension");
  //   esp_deep_sleep_start();
  // }
  }
}
=======
  // Modo funcionamiento NORMAL
  if (WiFi.status() == WL_CONNECTED && !modoconfig)
  {
    //Muestra con leds la carga de la batería
    battery();

    preferences.begin("myPreferences", true); // Sentencia para guardar valores en memoria no volatil

    leerCaraYGuardarValores();
    
    // Despertar al ESP32 según las condiciones
    despertarSegunCondiciones();

    // Comprobación del trabajo realizado y envío de datos
    manejarTrabajoRealizado();

    Serial.println ("holahola");
  }

  Serial.println ("fin del loop");
}
>>>>>>> Stashed changes
