#include <Arduino.h>
#include <LiquidCrystal.h>
// #include <EEPROM.h>

#include "mod.h"
#include "arm.h"
#include "protocol.h"

LiquidCrystal lcd(8, 9, 2, 3, 5, 6);

XBEEPacketSender XBEESender("RPM");
XBEEPacketReceiver XBEEReceiver("RPM", 19000);

armcoord_t currentCoordinates = {0, 0, 0, 0};

opmod_t operatingMode = MOD_MENU;
int lastArmPulse;

paktype_t lastPacket = PAKTYP_NONE;
pakdata_t lastData;

void setup(){
    Serial.begin(9600);

    lcd.begin(16, 2);
    lcd.clear();

    delay(500);

    lcd.home();
    lcd.print("XBArm (v1.0)");

    delay(500);

    lcd.setCursor(0, 1);
    lcd.print("Starting...");
    resetArm();
    lcd.clear();

    lastArmPulse = millis();
}

void fetchIncomingPacket(){
    pakdata_t data;
    paktype_t packet = XBEEReceiver.receivePacket(&data);

    if(packet == PAKTYP_FREE || packet == PAKTYP_RECORD) {
        lastPacket = packet;
        lastData = data;
        XBEESender.sendACK();
    }
    else if(packet == PAKTYP_BAD) {
        XBEESender.sendNACK();
    }
}

void loop(){
    fetchIncomingPacket();

    switch(operatingMode){
        case MOD_MENU:
        modMenu();
        break;

        case MOD_FREE:
        modFree();
        break;

        case MOD_JOYSTICK:
        //modJoystick();
        break;

        case MOD_RECORD:
        modRecord();
        break;

        case MOD_PLAY:
        modPlay();
        break;
    }

    if(millis() - lastArmPulse >= 20){
        manageArm();
        lastArmPulse = millis();
    }
}
