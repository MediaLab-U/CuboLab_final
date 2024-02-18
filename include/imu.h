// imu.h

#ifndef IMU_H
#define IMU_H

#include "configuration.h"

extern Adafruit_MPU6050 mpu;
extern sensors_event_t a, g, temp;

#define MPU6050_I2C_ADDRESS 0x68 

void initIMU();
void configIMU();
void readIMU();
void calculateSide();


#endif // IMU_H