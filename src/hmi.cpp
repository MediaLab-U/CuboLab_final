#include "hmi.h"

int notas[] = {1320, 1320, 0, 1320, 0, 1047, 1320, 0, 1568, 0, 0, 0, 784};
int notas2[] = {1320, 1320, 0, 1047, 0, 1320};
int notas3[] = {1047, 0, 1320, 0, 1568, 0, 2093, 0};

void initHMI() 
{
  pinMode(led_r, OUTPUT);
  pinMode(led_g, OUTPUT);
  pinMode(led_b, OUTPUT);
  ledsOff();
  ledcAttachPin(BUZZER_PIN, 0);
  pinMode(BUZZER_PIN, OUTPUT);

}

void ledBlue() {
  ledsOff();
  analogWrite(led_r, 255);
  analogWrite(led_g, 255);
  analogWrite(led_b, 0);
}

void ledGreen() {
  ledsOff();
  analogWrite(led_r, 255);
  analogWrite(led_g, 0);
  analogWrite(led_b, 255);
}

void ledRed() {
  ledsOff();
  analogWrite(led_r, 0);
  analogWrite(led_g, 255);
  analogWrite(led_b, 255);
}

void ledYellow(){
  ledsOff();
  analogWrite(led_r, 150);
  analogWrite(led_g, 0);
  analogWrite(led_b, 255);
}

void ledPurple(){
  ledsOff();
  analogWrite(led_r, 150);
  analogWrite(led_g, 255);
  analogWrite(led_b, 0);
}

void ledsOff() {
  analogWrite(led_r, 255);
  analogWrite(led_g, 255);
  analogWrite(led_b, 255);
}

void testBuzzer(){
  
  Serial.println("Tone test");
  tone(BUZZER_PIN, 500, 500);
}



void handleState(State state) {
  switch (state) {
    case NO_CONNECTION:                           // To-Do que no sea tan sonido a muerte
      ledsOff();
      for(int i = 0; i<2; i++){
        ledRed();
        tone(BUZZER_PIN, 500, 500);
        delayLab(500);
        ledsOff();
        delayLab(500);
        ledRed();
        tone(BUZZER_PIN, 500, 500);
        delayLab(500);
        ledsOff();
        delayLab(500);
      }
      break;

    case CORRECT_CONNECTION:
      ledsOff();
      break;

    case GREEN_BATTERY:
      ledsOff();
      ledGreen();
      delayLab(500);
      ledsOff();
      break;

    case YELLOW_BATTERY:
      ledsOff();
      ledYellow();
      delayLab(500);
      ledsOff();
      break;

    case RED_BATTERY:
      ledsOff();
      ledRed();
      delayLab(500);
      ledsOff();
      break;
    
    case NO_BATTERY:
      ledsOff();
      for(int i = 0;  i<1; i++){
        for(int j = 0;  j<2; j++){
        
        ledRed();
        //tone(BUZZER_PIN, 1000, 200);
        delayLab(200);
        ledsOff();
        delayLab(200);
        }
        
        delayLab(2000);
      }
      break;


    case GREEN_CHARGE:
      ledsOff();
      for (int brillo = 0; brillo <= 255; brillo++) {
        analogWrite(led_g, brillo);
        delayLab(10);
      }

      for (int brillo = 255; brillo >= 0; brillo--) {
        analogWrite(led_g, brillo);
        delayLab(10);
      }
      ledsOff();
      break;

    case CONFIG:
      ledsOff();
      ledRed();
      delayLab(500);
      ledsOff();
      ledGreen();
      delayLab(500);
      ledsOff();
      break;

      
    case NEW_SEND:
      ledBlue();
      tone(BUZZER_PIN, 440, 500);
      delayLab(2000);
      ledsOff();
      // 1 pitido
      break;
    
    case REMEMBER:
      for(int i = 0; i <3; i++){
        tone(BUZZER_PIN, 1000, 500);
        delayLab(500);
        tone(BUZZER_PIN, 1250, 500);
        delayLab(500);
        tone(BUZZER_PIN, 1500, 500);
        delayLab(2000);
      }
      break;

    case MARIO_BROS:
      // Reproducir la melodía
      for (int i = 0; i < 13; i++) {
        tone(BUZZER_PIN, notas[i]);
        delayLab(200);
        noTone(BUZZER_PIN);
        delayLab(50);
      }
      delayLab(2000);
      break;

    case UPDATED_OK:
      // Reproducir la melodía
      for (int i = 0; i < 6; i++) {
        tone(BUZZER_PIN, notas2[i]);
        if(i%2==0){
          ledsOff();
          ledRed();
        }else{
          ledsOff();
          ledPurple();
        }
        delayLab(200);
        noTone(BUZZER_PIN);
        delayLab(50);
      }
      
      ledsOff();
      ledGreen();
      delayLab(2000);
      break;

    case NEW_VERSION:
      // Reproducir la melodía
      for (int i = 0; i <8; i++) {
        tone(BUZZER_PIN, notas3[i]);
        if(i%2==0){
          ledsOff();
          ledGreen();
        }else{
          ledsOff();
          ledPurple();
        }
        delayLab(200);
        noTone(BUZZER_PIN);
        delayLab(50);
      }
      
      ledsOff();
      ledGreen();
      delayLab(2000);
      break;


    default:
      // Estado desconocido
      break;
  }
}


State readBatteryStateLab(){
  int batt= readBatteryPorcentage();

  if (batt >= 0 && batt <= 10) {
    return NO_BATTERY;
  } else if (batt >= 11 && batt <= 30) {
    return RED_BATTERY;
  } else if (batt >= 31 && batt <= 60) {
    return YELLOW_BATTERY;
  } else if (batt >= 61 && batt <= 100) {
    return GREEN_BATTERY;
  }else{
    return NO_BATTERY;
  }

}


