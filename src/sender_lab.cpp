#include "sender_lab.h"
#include "ads.h"
#include "wifi_lab.h"

HTTPClient http;

char macStr[18];


void sendData(){
    Serial.println("Send Data");
    int previousSide = preferences.getInt("PreviousSide", 0);

    if (previousSide != currentSide){
        Serial.println("Diferentes caras");
        Serial.print(previousSide);
        Serial.print(" -> ");
        Serial.println(currentSide);
        Serial.print("Battery: ");


        //readBattery();
        sendNewData();

        toneBuzz(0);

    }
    else{
        Serial.println("Misma cara");
    }

    preferences.putInt("PreviousSide", currentSide);
}

void sendNewData(){
    int batteryLevel = readBattery();

    String httpMessage = "https://www.unioviedo.es/medialab/datos_cube.php?e=" + String(currentSide) + "&m='" + (String)macStr + "'&b=" + String(batteryLevel);

    http.begin(httpMessage);
    int httpCode = http.GET();

    if (httpCode > 0){
        String payload = http.getString();
        Serial.println(payload);

        toneBuzz(0);
        
    }
    else{
        Serial.println("Error en la solicitud");
    }
        
}
