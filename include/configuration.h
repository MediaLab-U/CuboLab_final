// configuration.h
#ifndef CONFIGURATION_H
#define CONFIGURATION_H


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
#include <Adafruit_ADS1X15.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include "Time.h"

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

String getNextTime();

unsigned long timeToSeconds();

unsigned long timeDifference(String, String);

boolean isDay();

void delayLab(long);



#endif // CONFIGURATION_H