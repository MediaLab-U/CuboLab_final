#include "sender_lab.h"

HTTPClient http;


boolean sameSide(){
    Serial.println("Send Data");

    int previousSide = preferences.getInt("PreviousSide", 0);
    preferences.putInt("PreviousSide", currentSide);

    if (previousSide != currentSide){
        Serial.println("Diferentes caras");
        Serial.print(previousSide);
        Serial.print(" -> ");
        Serial.println(currentSide);
        Serial.println("Battery: ");

        return false;
        

    }
    else{
        Serial.println("Misma cara");
        
        return true;
    }

}

void sendData(){
    int batteryLevel = readBatteryLevel();
    
    http.setTimeout(10000); // Tiempo de espera en milisegundos
    // To-Do change to new function handleState
    handleState(NEW_SEND);
    String httpMessage = "https://www.unioviedo.es/medialab/datos_cube.php?e=" + String(currentSide) + "&m=%27" + (String)macStr + "%27&b=" + String(batteryLevel);
    Serial.println(httpMessage);
    http.begin(httpMessage);
    int httpCode = http.GET();
    Serial.print("HTTP Code:");
    Serial.println(httpCode);
    if (httpCode > 0){
        String payload = http.getString();
        Serial.println(payload);
        
    }
    else{
        Serial.println("Error en la solicitud");
    }
    

        
}
