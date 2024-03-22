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
  float Vmax = 8.4;
  float Vmin = 6.2;
  float ADC_max = 880;
  float ADC_min = 660;
  batteryLevel = (((ads1015.readADC_SingleEnded(0)-ADC_min)*(Vmax-Vmin))/(ADC_max-ADC_min))+Vmin;
  return batteryLevel;
}

int readBatteryPorcentage(){
  float limitsup = 880;
  float limitinf = 660;
  float limitetotal = (limitsup-limitinf);
  float lecturacortada = ((ads1015.readADC_SingleEnded(0)-limitinf));
  int batteryPorcentage = ((lecturacortada/limitetotal)*100);
  
  if (batteryPorcentage <0) {
    batteryPorcentage=0;
  }
  if (batteryPorcentage>100){
    batteryPorcentage=100;
  }
  return batteryPorcentage;

}

