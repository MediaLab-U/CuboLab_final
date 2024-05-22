#include "ads.h"


float batteryLevel = 0;
float batteryCell = 0;
int batteryPorcentage = 0;


float readBatteryLevel(){
  batteryLevel = analogRead(34);
  Serial.println(batteryLevel);
  if (batteryLevel<2618){batteryLevel = 2618;}
  batteryLevel = (batteryLevel - 2618) * 100;  //2618 Lectura minima analogica a 6.2 V

  batteryLevel = (batteryLevel/1182);          //3800 maximo a 8.4 V menos 2618 minimo a 6.2 V es igual a 1182

  return batteryLevel;
}

int readBatteryPorcentage(){
  int batteryPorcentage = readBatteryLevel();
  Serial.println(batteryPorcentage);
  if (batteryPorcentage <0) {
    batteryPorcentage=0;
  }
  if (batteryPorcentage>100){
    batteryPorcentage=100;
  }
  return batteryPorcentage;

}

