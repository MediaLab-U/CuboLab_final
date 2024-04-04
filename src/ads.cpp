#include "ads.h"


float batteryLevel = 0;
float batteryCell = 0;
int batteryPorcentage = 0;


float readBatteryLevel(){
  batteryLevel = analogRead(34);
  batteryLevel = batteryLevel * 5.6/2288;

  
  //float batteryCell1 = analogRead(32);
  //batteryCell1 = batteryCell1 * 2.02/1216;


  //batteryCell = batteryLevel - batteryCell1;
  return batteryLevel;
}

int readBatteryPorcentage(){
  int batteryPorcentage = readBatteryLevel()/8.4*100;
  if (batteryPorcentage <0) {
    batteryPorcentage=0;
  }
  if (batteryPorcentage>100){
    batteryPorcentage=100;
  }
  return batteryPorcentage;

}

