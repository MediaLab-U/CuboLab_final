#include "imu.h"

Adafruit_MPU6050 mpu; 
sensors_event_t a, g, temp;
int currentSide;

void initIMU(){
    
    if (!mpu.begin()) {
        // Manejar el fallo de inicialización
        Serial.println("MPU6050 INCORRECTO");
        while (1)
        {
        delay(1000);
        Serial.println("Reiniciando ESP32");
        esp_restart();
        }
    }

    Serial.println("MPU iniciado correctamente");
}

void configIMU(){

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    Serial.print("Accelerometer range set to: ");
    
    switch (mpu.getAccelerometerRange())
    {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
    }

    mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
    Serial.print("Gyro range set to: ");
    switch (mpu.getGyroRange())
    {
      case MPU6050_RANGE_250_DEG:
        Serial.println("+- 250 deg/s");
        break;
      case MPU6050_RANGE_500_DEG:
        Serial.println("+- 500 deg/s");
        break;
      case MPU6050_RANGE_1000_DEG:
        Serial.println("+- 1000 deg/s");
        break;
      case MPU6050_RANGE_2000_DEG:
        Serial.println("+- 2000 deg/s");
        break;
    }

    mpu.setFilterBandwidth(MPU6050_BAND_184_HZ);
    Serial.print("Filter bandwidth set to: ");
    
    switch (mpu.getFilterBandwidth())
    {
      case MPU6050_BAND_260_HZ:
        Serial.println("260 Hz");
        break;
      case MPU6050_BAND_184_HZ:
        Serial.println("184 Hz");
        break;
      case MPU6050_BAND_94_HZ:
        Serial.println("94 Hz");
        break;
      case MPU6050_BAND_44_HZ:
        Serial.println("44 Hz");
        break;
      case MPU6050_BAND_21_HZ:
        Serial.println("21 Hz");
        break;
      case MPU6050_BAND_10_HZ:
        Serial.println("10 Hz");
        break;
      case MPU6050_BAND_5_HZ:
        Serial.println("5 Hz");
        break;
    }

}

void readIMU(){
    mpu.getEvent(&a, &g, &temp);

    Serial.println("Read Data");
    Serial.print("AX: ");
    Serial.print(a.acceleration.x);
    Serial.print("AY: ");
    Serial.print(a.acceleration.y);
    Serial.print("AZ: ");
    Serial.println(a.acceleration.z);
}

void calculateSide(){

    int g = 9.8; // m/s^2

    // Comprueba si el valor de ax, ay o az está dentro del rango de ±2g
    if (a.acceleration.x > (g - 2) && a.acceleration.x < (g + 2)) {
        currentSide = 1; // Cara 1
    } else if (a.acceleration.x > (-g - 2) && a.acceleration.x < (-g + 2)) {
        currentSide = 2; // Cara 2
    } else if (a.acceleration.y > (g - 2) && a.acceleration.y < (g + 2)) {
        currentSide = 3; // Cara 3
    } else if (a.acceleration.y > (-g - 2) && a.acceleration.y < (-g + 2)) {
        currentSide = 4; // Cara 4
    } else if (a.acceleration.z > (g - 2) && a.acceleration.z < (g + 2)) {
        currentSide = 5; // Cara 5
    } else if (a.acceleration.z > (-g - 2) && a.acceleration.z < (-g + 2)) {
        currentSide = 6; // Cara 6
    } else {
        currentSide = 0; // Ninguna cara detectada
    }

    Serial.print("Cara actual: ");
    Serial.println(currentSide);
}