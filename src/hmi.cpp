#include "hmi.h"


void initHMI() 
{
  pinMode(led_g, OUTPUT);
  pinMode(led_b, OUTPUT);
  pinMode(led_r, OUTPUT);
  ledsOff();
  pinMode(BUZZER_PIN, OUTPUT);
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

void ledYellow(){
  digitalWrite(led_b, LOW);
  digitalWrite(led_g, HIGH);
  digitalWrite(led_g, HIGH);
}


void ledsOff() {
  digitalWrite(led_b, HIGH);
  digitalWrite(led_g, HIGH);
  digitalWrite(led_r, HIGH);
}



void handleState(State state) {
  switch (state) {
    case NO_CONNECTION:
      Serial.println("NO_CONNECTION");
      for(int i = 0; i<5; i++){
        ledRed();
        tone(BUZZER_PIN, 1000, 500); // Genera un tono de 1000 Hz durante 500 ms
        delayLab(500);
        ledsOff();
        delayLab(500);
      }
      break;

    case CORRECT_CONNECTION:
      Serial.println("CORRECT_CONNECTION");
      ledsOff();
      break;

    case GREEN_BATTERY:
      ledGreen();
      delayLab(500);
      ledsOff();
      break;

    case YELLOW_BATTERY:
      ledYellow();
      delayLab(500);
      ledsOff();
      break;

    case RED_BATTERY:
      ledRed();
      delayLab(500);
      ledsOff();
      break;
    
    case NO_BATTERY:
      for(int i = 0; i++; i<5){
        for(int j = 0; i++; i<2){
        ledRed();
        tone(BUZZER_PIN, 1000, 500); // Genera un tono de 1000 Hz durante 500 ms
        ledsOff();
        delayLab(500);
        }
        
        delayLab(2000);
      }
      break;

    case FULL_CHARGE:
      ledGreen();
      break;

    case GREEN_CHARGE:
      for (int brillo = 0; brillo <= 255; brillo++) {
        analogWrite(led_g, brillo);
        delayLab(10);
      }

      // Apagar gradualmente el LED
      for (int brillo = 255; brillo >= 0; brillo--) {
        analogWrite(led_g, brillo);
        delayLab(10);
      }
      break;

    case YELLOW_CHARGE:
      for (int brillo = 0; brillo <= 255; brillo++) {
        analogWrite(led_g, brillo);
        analogWrite(led_r, brillo);
        delayLab(10);
      }

      // Apagar gradualmente el LED
      for (int brillo = 255; brillo >= 0; brillo--) {
        analogWrite(led_g, brillo);
        analogWrite(led_r, brillo);
        delayLab(10);
      }
      break;

    case RED_CHARGE:
      for (int brillo = 0; brillo <= 255; brillo++) {
        analogWrite(led_r, brillo);
        delayLab(10);
      }

      // Apagar gradualmente el LED
      for (int brillo = 255; brillo >= 0; brillo--) {
        analogWrite(led_r, brillo);
        delayLab(10);
      }
      break;

    case WIFI_ERROR:
      // Luz roja parpadeando 5 veces
      // 1 pitido
      break;
    case NEW_SEND:
      ledBlue();
      tone(BUZZER_PIN, 1000, 500);
      ledsOff();
      // 1 pitido
      break;
    default:
      // Estado desconocido
      break;
  }
}


State readBatteryStateLab(boolean charge){
  int batt= readBatteryPorcentage();
  if(!charge){
    if (batt >= 0 && batt <= 10) {
      return NO_BATTERY;
    } else if (batt >= 11 && batt <= 30) {
      return RED_BATTERY;
    } else if (batt >= 31 && batt <= 60) {
      return YELLOW_BATTERY;
    } else if (batt >= 61 && batt <= 100) {
      return GREEN_BATTERY;
    }else{
      return ERROR;
    }
  }
  else{
    if (batt >= 0 && batt <= 30) {
      return RED_CHARGE;
    } else if (batt >= 31 && batt <= 60) {
      return YELLOW_CHARGE;
    } else if (batt >= 61 && batt <= 90) {
      return GREEN_CHARGE;
    } else if (batt >= 91 && batt <= 100) {
      return FULL_CHARGE;
    }
    else{
      return ERROR;
    }
  }

}


