#include "sleep_lab.h"

void goToSleep()
{

  Serial.println("CuboSleep");


  mpu.setMotionInterrupt(true);
  mpu.setMotionDetectionThreshold(3.0f);                 
  mpu.setMotionDetectionDuration(1);

  // Despertar al ESP32 cuando se conecte a la red
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 1);

  // Despertar al ESP32 cuando se mueva el MPU
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 1);


  // Apagar el ESP32 cuando no tenga suficiente batería
  // To-Do
  /*if (adc0 <= 0 && digitalRead(GPIO_NUM_4) != 1)
  {
    esp_deep_sleep_start();
  }*/

  // Desperter cada 2h (por defecto para recordar uso)
  long long int TIME_TO_SLEEP = 7200000000LL;
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP);
  esp_deep_sleep_start();
}

void configSleep() 
{
  // Activamos pines de interrupciones
  pinMode(4, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);

  // Habilitar la retención de pines durante el modo de bajo consumo
  gpio_deep_sleep_hold_en();
  // Deshabilitar temporizador preciso RTC durante deepsleep
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO);
} 