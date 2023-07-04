#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
// #include <Wire.h>
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
#include <TimeLib.h>
// Buzzer
//  #include <ezBuzzer.h>

#define led_g 26
#define led_b 25
#define led_r 27

//  #define buzzer_pin 35
//  Pines y dirección I2C
#define SDA_PIN 21
#define SCL_PIN 22
// #define MPU6050_I2C_ADDRESS 0x68 //Suele ser 0x68 o 0x69
#define pin_tension 4

Preferences preferences;

// Convertidor Analogico-Digital
Adafruit_ADS1015 ads;

// Buzzer
#define BUZZER_PIN 18
bool buzzer_on = false;
bool buzzer_flag = false;

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
// String ssid1 = "";
// String password1 = "";

String listaredes = "";
// int numredes;

// bool wifiScanCompleted = false;
bool scanningComplete = false;

WebServer server(80);

HTTPClient http;

Ticker timer;
Ticker wifiScanTicker;

char macStr[18];
byte mac[6];

int valor_cara = 0;
double timeini, timefin;
bool trabajoRealizado = false;
String actual;

void cierreconfig()
{
  pinMode(led_g, OUTPUT);
  pinMode(led_b, OUTPUT);
  pinMode(led_r, OUTPUT);
  // Código a ejecutar cuando se alcance el tiempo deseado
  //     WiFi.disconnect(true);
  // WiFi.begin(cuboSSID.c_str(), cuboPassword.c_str());
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  if (modoconfig == true)
  {
    digitalWrite(led_b, HIGH);
    digitalWrite(led_g, HIGH);
    digitalWrite(led_r, HIGH);
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

// void scanNetworks()
// {

//   wifiScanTicker.detach();
//   scanNetworksAsync();
// }

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
  pinMode(led_g, OUTPUT);
  pinMode(led_b, OUTPUT);
  pinMode(led_r, OUTPUT);
  digitalWrite(led_b, HIGH);
  digitalWrite(led_g, HIGH);
  digitalWrite(led_r, HIGH);
  cuboPassword = server.arg("password");
  server.send(200, "text/plain", "SSID y contraseña guardados: " + cuboSSID + ", " + cuboPassword);
  delay(2000);
  WiFi.disconnect(true);
  WiFi.begin(cuboSSID.c_str(), cuboPassword.c_str());
  int x1 = 0;
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
    x1 = x1 + 1;
    Serial.print(x1);
    if (x1 == 10)
    {
      analogWrite(led_b, 236);
      analogWrite(led_g, 100);
      analogWrite(led_r, 130);
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
      Serial.println("Modo punto de acceso iniciado");
      modoconfig = true;
      break;
    }
  }

  preferences.putString("ssid", cuboSSID);
  preferences.putString("pass", cuboPassword);

  // Serial.println("");
  // Serial.println("Conexión establecida");
  // Serial.print("Dirección IP: ");
  // Serial.println(WiFi.localIP());
  modoconfig = false;
  timer.detach();
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
  // Wire.begin();

  // Try to initialize!
  mpu.begin();
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

  // COmprobacion de q funciona (A Ramón no le mola esto)
  ads.begin();
  if (!ads.begin())
  {
    Serial.println("Failed to initialize ADS.");
    // while (1)
    //   ;
  }

  preferences.begin("myPreferences", false);

  // sistema de archivos para imagenes de la pagina de config
  LittleFS.begin();

  // Comenzar conexión I2C
  // Wire.begin(SDA_PIN, SCL_PIN);

  // Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  // Wire.write(0x6B); // Dirección del registro de configuración del MPU6050
  // WiFi.scanNetworks(onScanComplete);

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);

  // Conexión a la red WiFi
  String ssid1 = preferences.getString("ssid", "medialab");
  String password1 = preferences.getString("pass", "medialab");
  WiFi.begin(ssid1.c_str(), password1.c_str());

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

  // DESPERTAR EL ESP CUANDO SE CONECTA A LA FUENTE DE ALIMENTACIÓN
  // esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 1);

  Serial.print("Conectando a la red WiFi.");
  delay(100);
  Serial.print(".");
  delay(100);
  Serial.println(".");

  int x = 0;
  while ((WiFi.status() != WL_CONNECTED))
  {
    digitalWrite(led_b, LOW);
    digitalWrite(led_g, HIGH);
    digitalWrite(led_r, HIGH);
    delay(800);
    digitalWrite(led_b, HIGH);
    digitalWrite(led_g, HIGH);
    digitalWrite(led_r, HIGH);
    delay(800);

    // if (WiFi.status() == WL_CONNECTED)
    // {
    //   break;
    // }

    x = x + 1;
    Serial.print(x);

    if (digitalRead(pin_tension) == HIGH && modoconfig == false && iniconfig == false)
    {
      break;
    }

    if (x == 10)
    {
      digitalWrite(led_g, HIGH);
      digitalWrite(led_r, LOW);
      digitalWrite(led_b, HIGH);
      delay(1000);
      digitalWrite(led_g, HIGH);
      digitalWrite(led_r, HIGH);
      digitalWrite(led_b, HIGH);
      Serial.println("No se ha podido conectar a la red WIFI");
      esp_deep_sleep_start();
    }
    //////////////////////////////////////////////////
    // Convertidor Analogico-Digital
  }
  // Convertidor Analogico-Digital

  digitalWrite(led_g, HIGH);
  digitalWrite(led_r, HIGH);
  digitalWrite(led_b, HIGH);

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Conexión exitosa");

    // Obtención de la dirección MAC del ESP32
    WiFi.macAddress(mac);

    // Conversión de la dirección MAC a una cadena de caracteres
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    mac[0] = 0;

    // while (!Serial)
    //   delay(1000); // will pause Zero, Leonardo, etc until serial console opens

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
    // attachInterrupt(digitalPinToInterrupt(4), cargando, RISING);
  }
  if (modoconfig == false)
  {
    mpu.setMotionInterrupt(true);
    pinMode(14, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(14), handleInterrupt, RISING);
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

void reg_horario()
{
  const int startHour = 9;
  const int endHour = 21;
  int currentHour = hour();
  Serial.println(currentHour);
  if (currentHour >= startHour && currentHour < endHour)
  {
    buzzer_on = true;
  }
  else
  {
    buzzer_on = false;
  }
}

void battery()
{
  adc0 = ((ads.readADC_SingleEnded(0) - 745) * 100) / (948 - 745); // leemos el valor analógico presente en el pin
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
    digitalWrite(led_r, LOW);
    digitalWrite(led_g, HIGH);
    digitalWrite(led_b, HIGH);
    if (buzzer_flag == true)
    {
      tone(BUZZER_PIN, 800);
      delay(100);
      tone(BUZZER_PIN, 0);
      delay(100);
      tone(BUZZER_PIN, 800);
      delay(100);
      tone(BUZZER_PIN, 0);
      buzzer_flag = false;
    }

  case 10 ... 30:
    digitalWrite(led_r, LOW);
    digitalWrite(led_g, HIGH);
    digitalWrite(led_b, HIGH);
    break;
  case 31 ... 80:
    digitalWrite(led_r, HIGH);
    digitalWrite(led_g, HIGH);
    digitalWrite(led_b, LOW);
    break;
  default:
    digitalWrite(led_r, HIGH);
    digitalWrite(led_g, LOW);
    digitalWrite(led_b, HIGH);
    break;
  }
  Serial.println(adc0);
  delay(1000);
}

void bateria(int *porcentaje)
{
  int porcent = (((ads.readADC_SingleEnded(0) - 745) * 100) / (948 - 745)); // leemos el valor analógico presente en el pin
  if (porcent >= 100)
    *porcentaje = 100;
  else if (porcent <= 0)
    *porcentaje = 0;
  else
    *porcentaje = porcent;
}

void loop()
{
  // Loop del buzzer
  //  buzzer.loop();
  //
  server.handleClient();
  // Serial.println(macStr);
  //  Código indicador de batería//
  /*****************************/

  if (digitalRead(pin_tension) == HIGH && modoconfig == false && iniconfig == false) // entrar en modo config cuando se conecta al cargador
  {
    analogWrite(led_b, 236);
    analogWrite(led_g, 100);
    analogWrite(led_r, 130);
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
  else if (digitalRead(pin_tension) == LOW && iniconfig == true)
  {
    iniconfig = false; // cierre de la variable de inicio al ser desconectado
  }

  // Modo funcionamiento normal
  if (WiFi.status() == WL_CONNECTED && modoconfig == false)
  {
    battery();
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
    // // Despertar al ESP32 cuando se conecte a la red
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 1);
    // Apagar el ESP32 cuando no tenga suficiente batería
    if (adc0 <= 0 && digitalRead(GPIO_NUM_4) != 1)
    {
      Serial.println("No tengo tension");
      esp_deep_sleep_start();
    }
    while (digitalRead(GPIO_NUM_4) == 1)
    {
      battery();
    }

    // Despertar al ESP32 cuando se mueva el MPU
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 1);

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

        // Buzzer
        reg_horario();
        Serial.println(buzzer_on);
        if (buzzer_on == HIGH)
        {
          tone(BUZZER_PIN, 1000); // 8KHz
          delay(500);
        }
        // Cierre Buzzer

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
      // digitalWrite(led_g, HIGH);
      // digitalWrite(led_r, HIGH);
      // digitalWrite(led_b, HIGH);
      // Código para enviar a dormi el SP32 y despertarlo cada 15 minutos
      Serial.println("Me voy a dormir");

      esp_deep_sleep_start();
    }
  }
}