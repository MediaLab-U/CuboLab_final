#include "ads.h"


Adafruit_ADS1015 ads1015;

void initADS(){
  if (!ads1015.begin())
    {
      Serial.println("Fallo del analógico-digital");
    }
}

int readBattery(){
  int adc0 = ads1015.readADC_SingleEnded(0);
  Serial.println(adc0);
  
  return adc0;
}