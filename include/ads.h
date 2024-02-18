// ads.h

#ifndef ADS_H
#define ADS_H

#include "configuration.h"

extern Adafruit_ADS1015 ads1015;
extern int batteryLevel;
extern int batteryPorcentage;

#define ADS1X15_ADDRESS (0x48)   

void initADS();

float readBatteryLevel();

int readBatteryPorcentage();


#endif // ADS_H