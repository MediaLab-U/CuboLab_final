#include "ads.h"

Adafruit_ADS1015 ads1015;

int batteryLevel = 0;
int batteryPorcentage = 0;

void initADS(){
  if (!ads1015.begin())
    {
      Serial.println("Fallo del analógico-digital");
    }
}

float readBatteryLevel(){
  // TO-DO
  batteryLevel = ads1015.readADC_SingleEnded(0);
  return 4.0;
}

int readBatteryPorcentage(){
  batteryPorcentage = batteryLevel * 100 /4.2;
  return batteryPorcentage;
}

