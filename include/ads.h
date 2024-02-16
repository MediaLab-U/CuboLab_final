// ads.h

#ifndef ADS_H
#define ADS_H

#include <Arduino.h>
#include "configuration.h"


extern Adafruit_ADS1015 ads1015;

#define ADS1X15_ADDRESS (0x48)   

void initADS();

int readBattery();

#endif // ADS_H