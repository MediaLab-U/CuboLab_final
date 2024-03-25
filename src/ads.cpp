#include "ads.h"


float batteryLevel = 0;
int batteryPorcentage = 0;


float readBatteryLevel(){
  float Vmax = 8.4;
  float Vmin = 6.2;
  batteryLevel = analogRead(VBat);
  batteryLevel = batteryLevel/4096*3.3 *34/12;
  return batteryLevel;
}

int readBatteryPorcentage(){

  int batteryPorcentage = readBatteryLevel()*100/8.4;
  
  if (batteryPorcentage <0) {
    batteryPorcentage=0;
  }
  if (batteryPorcentage>100){
    batteryPorcentage=100;
  }
  return batteryPorcentage;

}

