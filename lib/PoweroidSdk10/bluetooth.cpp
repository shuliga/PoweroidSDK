#include "bluetooth.h"

Bt::Bt(){}

Bt::Bt(char * n): name(n){
    if (Serial && !on){
        Serial.println(F("Looking for Bluetooth HC-06"));
        delay(6000);
        Serial.print(F("AT+VERSION"));
        delay(600);
        if (Serial.available() > 0){
            String ver = Serial.readString();
            result = "BT version: " + ver;
            if (ver.startsWith(VER) ){
                delay(600);
                Serial.print(F("AT+NAME"));
                Serial.print(name);
                on = true;
                result = "BT name set to: " + String(name);
                delay(600);
            }
        } else {
            Serial.println("");
            result = "No BT version data.";
        }
        Serial.println(result);
    }
}

void Bt::getResult(){
    Serial.println(result);
}

void Bt::setPin(char * pin){
    if (Serial && on){
        Serial.print(F("AT+PIN"));
        Serial.print(pin);
        delay(600);
    }
}

bool Bt::isOn(){
    return on;
}
