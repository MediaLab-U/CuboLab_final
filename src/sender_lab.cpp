#include "sender_lab.h"

HTTPClient http;


boolean sameSide(){

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
    int batteryLevel = readBatteryPorcentage();
    
    http.setTimeout(10000); // Tiempo de espera en milisegundos
    handleState(NEW_SEND);
    getMac();
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

    getTime();
    preferences.putString("lastTime", cubeTime);
    cuboSleep = 7200000000LL;
        
}

void sendBattery(){
    Serial.println("Send Battery Info");
    int batteryLevel = readBatteryPorcentage();
    
    http.setTimeout(15000); // Tiempo de espera en milisegundos

    String httpMessage = "https://www.unioviedo.es/medialab/datos_cube.php?m=%27" + (String)macStr + "%27&b=" + String(batteryLevel);
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

    getTime();
    preferences.putString("lastTime", cubeTime);
    Serial.print("Cubo Time last: ");
    Serial.println(cubeTime);
    cuboSleep = 7200000000LL;

    
}