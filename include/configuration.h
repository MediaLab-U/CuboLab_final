// configuration.h
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define FIRMWARE_VERSION "0.1"

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <esp_sleep.h>
#include <Preferences.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include "I2Cdev.h"
#include <WebServer.h>
#include <LITTLEFS.h>
#include "Time.h"
#include <WiFiMulti.h>

#include <Arduino.h>

#include "hmi.h"
#include "ads.h"
#include "imu.h"
#include "sender_lab.h"
#include "sleep_lab.h"
#include "web_lab.h"
#include "wifi_lab.h"

extern Preferences preferences;
extern int currentSide;

extern boolean firstConfig;
extern String cubeTime;

extern long long int cuboSleep;

extern float cubeVersion;
extern float version;

#define VBat    34
#define VCharge 33

#define SDA_PIN 21
#define SCL_PIN 22

enum CubeState {
    WIFI_CONFIG,
    CHARGE,
    NORMAL_MODE,
};

extern CubeState cubeState;

void initMemory();

void configFileSystem();

void configMode();

void configTime();

boolean getTime();

void timeToSleep();

boolean isDay();

void delayLab(long);



#endif // CONFIGURATION_H