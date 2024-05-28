// imu.h

#ifndef HMI_H
#define HMI_H

#include "configuration.h"

#define led_g 26
#define led_b 27
#define led_r 25

#define BUZZER_PIN 18

enum State {
  NO_CONNECTION,
  CORRECT_CONNECTION,
  GREEN_BATTERY,
  YELLOW_BATTERY,
  RED_BATTERY,
  GREEN_CHARGE,
  CONFIG,
  NO_BATTERY,
  NEW_SEND,
  REMEMBER,
  MARIO_BROS,
  UPDATED_OK,
  NEW_VERSION,
};

void initHMI();

void ledBlue();
void ledGreen();
void ledYellow();
void ledPurple();
void ledRed();
void ledsOff();

void testBuzzer();

void handleState(State);

State readBatteryStateLab();


#endif // IMU_H