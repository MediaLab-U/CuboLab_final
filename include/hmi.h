// imu.h

#ifndef HMI_H
#define HMI_H

#include <Arduino.h>

#define led_g 26
#define led_b 25
#define led_r 27

#define BUZZER_PIN 18

void initLeds();
void ledBlue();
void ledGreen();
void ledRed();
void turnOffLeds();

void initBuzz();
void toneBuzz(int);


#endif // IMU_H