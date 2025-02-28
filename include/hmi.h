// imu.h

#ifndef HMI_H
#define HMI_H

#include "configuration.h"

#define led_r 25
#define led_g 26
#define led_b 27

#define BUZZER_PIN 18

enum State {
  NO_CONNECTION,
  CORRECT_CONNECTION,
  GREEN_BATTERY,
  YELLOW_BATTERY,
  RED_BATTERY,
  FULL_CHARGE,
  GREEN_CHARGE,
  YELLOW_CHARGE,
  RED_CHARGE,
  NO_BATTERY,
  NEW_SEND,
  CONFIG,
  REMEMBER,
  MARIO_BROS,
};

void initHMI();
void testBuzzer();
void ledBlue();
void ledGreen();
void ledYellow();
void ledPurple();
void ledRed();
void ledsOff();

void handleState(State);

State readBatteryStateLab(boolean charge = false);


#endif // IMU_H