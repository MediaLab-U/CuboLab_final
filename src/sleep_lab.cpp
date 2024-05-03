#include "sleep_lab.h"

void goToSleep()
{
  mpu.setMotionInterrupt(true);
  mpu.setMotionDetectionThreshold(5.0f);                // To-Do Sensibilidad del cubo               
  mpu.setMotionDetectionDuration(1);

  // Despertar al ESP32 cuando se conecte a la red
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1);


  // Despertar al ESP32 cuando se mueva el MPU
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 1);


  // Desperter cada 2h (por defecto para recordar uso)
  Serial.print("cuboSleep sleep:");
  Serial.println(cuboSleep);
  esp_sleep_enable_timer_wakeup(cuboSleep);

  esp_deep_sleep_start();
}

// Apagar el ESP32 cuando no tenga suficiente batería
void sleep_no_batt(){
    // Despertar al ESP32 cuando se conecte a la red
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1);
  
  esp_deep_sleep_start();
}

void configSleep() 
{
  // Activamos pines de interrupciones
  pinMode(33, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);

  // Habilitar la retención de pines durante el modo de bajo consumo
  gpio_deep_sleep_hold_en();
  // Deshabilitar temporizador preciso RTC durante deepsleep
  //esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO);

} 