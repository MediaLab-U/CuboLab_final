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
  return batteryLevel * 5 / 536;
}

int readBatteryPorcentage(){

  if (batteryLevel < 2.1){
    batteryPorcentage = 0;
  }
  else if (batteryLevel > 2.6){
    batteryPorcentage = 100;
  }
  else{
    batteryPorcentage = ((batteryLevel - 2.1) / (2.6 - 2.1)) * 100;
  }
  
  return batteryPorcentage;
}

