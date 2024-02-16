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

extern Preferences preferences;
extern int currentSide;

extern boolean  modoConfig;
extern boolean firstConfig;


#define SDA_PIN 21
#define SCL_PIN 22

void initMemory();

void configFileSystem();

void configTime();



#endif // CONFIGURATION_H