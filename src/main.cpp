#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <esp_sleep.h>
#include <Preferences.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "I2Cdev.h"
#include <WebServer.h>
#include "ticker.h"
#include <LITTLEFS.h>

// Convertidor Analogico-Digital y comunicacion I2C
#include <Adafruit_ADS1X15.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>

// Horario
#include "Time.h"

#define led_g 26
#define led_b 25
#define led_r 27

//  Pines y dirección I2C
#define SDA_PIN 21
#define SCL_PIN 22

// #define MPU6050_I2C_ADDRESS 0x68 //Suele ser 0x68 o 0x69
#define pin_tension 4


const int SENSIBILIDAD_SENSOR = 8;
const int NUM_INTENTOS_WIFI = 10;


Preferences preferences;

// Convertidor Analogico-Digital
Adafruit_ADS1015 ads;

// Buzzer
#define BUZZER_PIN 18
bool buzzer_on = false;
bool buzzer_flag = true;

#define beep_battery 0
#define beep_2h 1
#define beep_send 2

// Horario
int hora;

Adafruit_MPU6050 mpu;

int16_t adc0;
#define ADS1X15_ADDRESS (0x48)   /// 1001 000 (ADDR = GND) ads1115 I2C adress
#define MPU6050_I2C_ADDRESS 0x68 // I2C adress mpu

int flag = false;
int modoconfig = false;
int iniconfig = false;
int voltaje;

const char *ssid = "ConfigCUBO";
const char *password = "cubolab00";
// Variables donde se guardan los datos del wifi
String cuboSSID = "";
String cuboPassword = "";

String listaredes = "";

bool scanningComplete = false;

WebServer server(80);

HTTPClient http;

Ticker timer;
Ticker wifiScanTicker;
// Ticker remember; // envio cada 2 horas de un pitido para recordar el uso del cubo

char macStr[18];
byte mac[6];

int valor_cara = 99; //Inicializamos valor_cara a 99. Los valores válidos son entre 0 y 5.
double timeini, timefin;
bool trabajoRealizado = false;
String actual;



void ledAzul()
{
  digitalWrite(led_b, LOW);
  digitalWrite(led_g, HIGH);
  digitalWrite(led_r, HIGH);
}

void ledVerde()
{
  digitalWrite(led_b, HIGH);
  digitalWrite(led_g, LOW);
  digitalWrite(led_r, HIGH);
}

void ledRojo()
{
  digitalWrite(led_b, HIGH);
  digitalWrite(led_g, HIGH);
  digitalWrite(led_r, LOW);
}

void ledNaranja()
{
  analogWrite(led_b, 220);
  analogWrite(led_g, 130);
  analogWrite(led_r, 120);
}

void ledAmarillo() //27/10/23_Añadido Ramon.
{
  analogWrite(led_b, LOW);
  analogWrite(led_g, LOW);
  analogWrite(led_r, HIGH);
}

void ledBlanco()
{
  digitalWrite(led_b, LOW);
  digitalWrite(led_g, LOW);
  digitalWrite(led_r, LOW);
}

void ledApagado()
{
  digitalWrite(led_b, HIGH);
  digitalWrite(led_g, HIGH);
  digitalWrite(led_r, HIGH);
}

// Hora en el ESP32
void setTimezone(String timezone)
{
  Serial.printf("  Setting Timezone to %s\n", timezone.c_str());
  setenv("TZ", timezone.c_str(), 1); //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

void initTime(String timezone)
{
  struct tm timeinfo;

  Serial.println("Setting up time");
  configTime(0, 0, "hora.roa.es"); // First connect to NTP server, with 0 TZ offset
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println("Got the time from NTP");
  // Now we can set the real timezone
  setTimezone(timezone);
}

void beep_buzzer(int modo)
{

  switch (modo)
  {
  case 0:
    if (buzzer_flag == true)
    {
      analogWrite(GPIO_NUM_18, 80);
      delay(100);
      analogWrite(GPIO_NUM_18, 0);
      delay(100);
      analogWrite(GPIO_NUM_18, 80);
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
  Serial.println("temporizador terminado");
  timer.detach();
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

void handleGif()
{
  File imageFile1 = LittleFS.open("/carga.gif", "r");
  if (imageFile1)
  {
    server.streamFile(imageFile1, "image/gif");
    imageFile1.close();
  }
}

void escanredes()
{
  Serial.println("Iniciando lectura de redes");
  // delay(1000); // Agregar un retraso de 1 segundo
  int numredes = WiFi.scanNetworks();
  Serial.println(numredes);

  for (int i = 0; i < numredes; i++)
  {
    String ssidd = WiFi.SSID(i);
    Serial.print(".");
    listaredes += "<li><a href='/select?ssid=" + ssidd + "'>" + ssidd + "</a></li>";
    Serial.print("Leyendo red: ");
    Serial.println(ssidd);
  }

  Serial.println("Finalizado lectura");

  scanningComplete = true;
  // wifiScanCompleted = true;
  wifiScanTicker.detach();
}

///////////////////////////////////////////////
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
////////////////////////////////////////////////////////////

void handleSelect()
{
  cuboSSID = server.arg("ssid");
  server.send(200, "text/html", "SSID seleccionado: " + cuboSSID + "\nPor favor, ingresa la contraseña correspondiente: <form method='POST' action='/save'><input type='password' name='password'><input type='submit' value='Guardar'></form>");

  Serial.println("ssid guardado");
}

void handleSave()
{
  pinMode(led_b, OUTPUT);
  pinMode(led_r, OUTPUT);
  pinMode(led_g, OUTPUT);

  cuboPassword = server.arg("password");
  server.send(200, "text/plain", "SSID y contraseña guardados: " + cuboSSID + ", " + cuboPassword);
  delay(2000);
  WiFi.disconnect(true);
  WiFi.begin(cuboSSID.c_str(), cuboPassword.c_str());
  int x1 = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    ledAzul();
    delay(800);
    ledApagado();
    delay(800);
    x1 = x1 + 1;
    Serial.print(x1);
    if (x1 == 10)
    {
      // ledNaranja();
      Serial.println("No se ha podido conectar a la red WIFI");
      WiFi.disconnect(true);
      WiFi.softAP(ssid, password);
      delay(100);
      server.on("/carga.gif", handleGif);
      server.on("/", handleRoot);
      server.on("/select", handleSelect);
      server.on("/save", handleSave);
      server.on("/cubolab.jpg", handleImage);
      server.begin();
      timer.attach(120.0, loop); // tiempo en segundos, al terminar ejecutará la funcion cierreconfig
      Serial.println("Modo punto de acceso iniciado");
      modoconfig = true;
      break;
    }
  }

  preferences.putString("ssid", cuboSSID);
  preferences.putString("pass", cuboPassword);

  modoconfig = false;
  timer.detach();
}

void IRAM_ATTR handleInterrupt()
{
  Serial.println("MOVIMIENTO DETECTADO!!!");
}


void inicializarMPU() {
  if (!mpu.begin()) {
    // Manejar el fallo de inicialización
    Serial.println("MPU6050 INCORRECTO");
    while (1)
    {
      delay(1000);
      Serial.println("Reiniciando ESP32");
      esp_restart();
    }
  }
  Serial.println("MPU6050 funcionando correctamente.");
}

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
  {
    ledAzul();
    delay(tiempoEspera);
    ledApagado();
    delay(tiempoEspera);

    if (WiFi.status() == WL_CONNECTED) 
    {
      Serial.println("\nConexión exitosa");
      return;
    }
  }
}

void configurarMPU() 
{
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Conexión exitosa");

    // Obtención de la dirección MAC del ESP32
    WiFi.macAddress(mac);

    // Conversión de la dirección MAC a una cadena de caracteres
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    mac[0] = 0;
    Serial.println(macStr);

    Serial.println("Adafruit MPU6050 test!");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    Serial.println("seleccionado rango");
    Serial.print("Accelerometer range set to: ");
    
    switch (mpu.getAccelerometerRange())
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

  }
}

void configurarHora() {
    // Start time
    Serial.setDebugOutput(true);
    initTime("CET-1CEST,M3.5.0/1,M10.5.0");
}

void activarBuzzer() {
    // Buzzer wakeup
    beep_time();
}

//hola
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
}

double Ctimer(void)
{
  struct timeval tm;

  gettimeofday(&tm, NULL);

  return tm.tv_sec + tm.tv_usec / 1.0E6;
}

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
    
    //Pongo elseif para que no entre si detectó un cambio   
    //------------EJE X-------------------//
    // ------------EJE X ramon------------------- hay que estructurar esto con una función que englobe los rtes ejes //
    
    /* Debajo he puesto código optimizado, tengo que ver si funciona
    int x = (int)a.acceleration.x;
    int y = (int)a.acceleration.y;
    int z = (int)a.acceleration.z;

    if (abs(x) > SENSIBILIDAD_SENSOR) 
    {
      timeini = Ctimer();
      unsigned long tiempoEspera = 3000;  // 3 segundos de espera
      unsigned long tiempoInicio = millis();
      unsigned long tiempoUltimaLectura = tiempoInicio;

      while (millis() - tiempoInicio <= tiempoEspera) 
      {
        mpu.getEvent(&a, &g, &temp);
        int newX = (int)a.acceleration.x;

        if (newX * x <= 0 || abs(newX) < SENSIBILIDAD_SENSOR) {
          break;
        }
        if (millis() - tiempoUltimaLectura >= 1000) {
          tiempoUltimaLectura = millis();
          // Aquí puedes realizar acciones adicionales que necesiten ejecutarse cada segundo
        }
      }

      timefin = Ctimer() - timeini;

      if (timefin > 3) 
      {
        timeini = 0;
        timefin = 0;
        actual = (x > 0) ? "Estoy muy feliz" : "Estoy muy mal";
        valor_cara = (x > 0) ? 2 : 4;
        trabajoRealizado = true;
      }
    }

    //------------EJE Y-------------------//
    else if (abs(y) > SENSIBILIDAD_SENSOR)
    {
      timeini = Ctimer();
      if (y > 0 && valor_cara != 3)
      {
        valor_cara = 3;
        while (1)
        {
          mpu.getEvent(&a, &g, &temp);
          //int y = (int)a.acceleration.y;
          delay(1000);
          if (y < 0 || abs(y) < SENSIBILIDAD_SENSOR || (Ctimer() - timeini) > 3)
          {
            break;
          }
        }
        timefin = Ctimer() - timeini;

        if (timefin > 3)
        {
          timeini = 0;
          timefin = 0;
          actual = "Estoy mal";
          Serial.println(actual);
          trabajoRealizado = true;
        }
      }
      else if (y < 0 && valor_cara != 1)
      {
        valor_cara = 1;
        while (1)
        {
          mpu.getEvent(&a, &g, &temp);
          //int y = (int)a.acceleration.y;
          delay(1000);
          if (y > 0 || abs(y) < SENSIBILIDAD_SENSOR || (Ctimer() - timeini) > 3)
          {
            break;
          }
        }
        timefin = Ctimer() - timeini;

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
    else if (abs(z) > SENSIBILIDAD_SENSOR)
    {
      
      timeini = Ctimer();
      if (z > 0 && valor_cara != 5)
      {
        valor_cara = 5;
        while (1)
        {
          mpu.getEvent(&a, &g, &temp);
          //int z = (int)a.acceleration.z;
          delay(1000);
          if (z < 0 || abs(z) < SENSIBILIDAD_SENSOR || (Ctimer() - timeini) > 3)
          {
            break;
          }
        }
        timefin = Ctimer() - timeini;

        if (timefin > 3)
        {
          timeini = 0;
          timefin = 0;
          trabajoRealizado = true;
        }
      }
      else if (z < 0 && valor_cara != 0)
      {
        valor_cara = 0;
        while (1)
        {
          mpu.getEvent(&a, &g, &temp);
          //int z = (int)a.acceleration.z;
          delay(1000);
          if (z > 0 || abs(z) < SENSIBILIDAD_SENSOR || (Ctimer() - timeini) > 3)
          {
            break;
          }
        }
        timefin = Ctimer() - timeini;

        if (timefin > 3)
        {
          timeini = 0;
          timefin = 0;
          actual = "Estoy ni bien ni mal";

          trabajoRealizado = true;
        }
      }
    }
    */



  int valores[3] = {(int)a.acceleration.x, (int)a.acceleration.y, (int)a.acceleration.z};
  String mensajes[] = {"Estoy muy feliz", "Estoy feliz", "Estoy ni bien ni mal", "Estoy mal", "Estoy bastante mal", "Estoy muy mal"};
  int ejes[] = {0, 1, 2, 3, 4, 5}; // Correspondencia con los valores_cara

  for (int i = 0; i < 3; ++i) 
  {
    int valor = valores[i];
    if (abs(valor) > SENSIBILIDAD_SENSOR) 
    {
      timeini = Ctimer();
      unsigned long tiempoEspera = 3000;  // 3 segundos de espera
      unsigned long tiempoInicio = millis();
      unsigned long tiempoUltimaLectura = tiempoInicio;

      while (millis() - tiempoInicio <= tiempoEspera) 
      {
        mpu.getEvent(&a, &g, &temp);
        int nuevoValor = valores[i];

        if (nuevoValor * valor <= 0 || abs(nuevoValor) < SENSIBILIDAD_SENSOR) 
        {
          break;
        }
        if (millis() - tiempoUltimaLectura >= 1000) 
        {
          tiempoUltimaLectura = millis();
          // Aquí puedes realizar acciones adicionales que necesiten ejecutarse cada segundo
        }
      }

      timefin = Ctimer() - timeini;

      if (timefin > 3) 
      {
        timeini = 0;
        timefin = 0;
        actual = mensajes[ejes[i]];
        valor_cara = ejes[i];
        trabajoRealizado = true;
      }
    }
  }

  Serial.println ("Valor cara: ");
  Serial.println (valor_cara);
  /***********************/
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

void loop()
{
  server.handleClient();

  //  Código indicador de batería//
  /*****************************/
  if (digitalRead(pin_tension) == HIGH && modoconfig == false && iniconfig == false) // entrar en modo config cuando se conecta al cargador
  {
    modoConfiguracion();
  }
  else if (digitalRead(pin_tension) == LOW && iniconfig == true)
  {
    iniconfig = false; // cierre de la variable de inicio al ser desconectado
  }

  // Modo funcionamiento NORMAL
  if (WiFi.status() == WL_CONNECTED && modoconfig == false)
  {
    //Muesta con leds la carga de la batería
    battery();

    /***********************/
    // Cierre código indicador de batería

    preferences.begin("myPreferences", true); // Sentencia para guardar valores en memoria no volatil

    leerCaraYGuardarValores();
    
    // Despertar al ESP32 según las condiciones
    despertarSegunCondiciones();

    // Comprobación del trabajo realizado y envío de datos
    manejarTrabajoRealizado();


  }

}