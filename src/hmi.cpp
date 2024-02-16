#include "hmi.h"


void initLeds() 
{
  pinMode(led_g, OUTPUT);
  pinMode(led_b, OUTPUT);
  pinMode(led_r, OUTPUT);
  turnOffLeds();
}

void ledBlue() {
  digitalWrite(led_b, LOW);
  digitalWrite(led_g, HIGH);
  digitalWrite(led_r, HIGH);
}

void ledGreen() {
  digitalWrite(led_b, HIGH);
  digitalWrite(led_g, LOW);
  digitalWrite(led_r, HIGH);
}

void ledRed() {
  digitalWrite(led_b, HIGH);
  digitalWrite(led_g, HIGH);
  digitalWrite(led_r, LOW);
}


void turnOffLeds() {
  digitalWrite(led_b, HIGH);
  digitalWrite(led_g, HIGH);
  digitalWrite(led_r, HIGH);
}

void initBuzz(){
  pinMode(BUZZER_PIN, OUTPUT);
}

void toneBuzz(int mode){
  switch (mode)
  {
  case 0 :
    analogWrite(BUZZER_PIN, 255);
    delay(100);
    analogWrite(BUZZER_PIN, 0);
    delay(100);
    analogWrite(BUZZER_PIN, 255);
    delay(100);
    analogWrite(BUZZER_PIN, 0);
    break;
  
  default:
    analogWrite(BUZZER_PIN, 255);
    delay(100);
    analogWrite(BUZZER_PIN, 0);
    delay(100);
    analogWrite(BUZZER_PIN, 255);
    delay(100);
    analogWrite(BUZZER_PIN, 0);
    delay(100);
    analogWrite(BUZZER_PIN, 255);
    delay(100);
    analogWrite(BUZZER_PIN, 0);
    break;
  }

}


