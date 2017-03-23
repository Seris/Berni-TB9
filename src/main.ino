#include <Arduino.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#include "arm.h"
#include "protocol.h"

LiquidCrystal lcd(8, 9, 2, 3, 5, 6);

XBEEPacketSender XBEESender("RPM");
XBEEPacketReceiver XBEEReceiver("RPM", 19000);

opmod_t operatingMode = MOD_MENU;
int lastArmPulse;

paktype_t lastPacket = PAKTYP_NONE;
pakdata_t lastData;

armcoord_t currentCoordinates = {0, 0, 0};

void setup(){
    lcd.begin(16, 2);
    lcd.clear();
    lcd.home();

    lcd.print("Starting...");
    resetArm();
    lcd.clear();

    lastArmPulse = millis();
}

void fetchIncomingPacket(){
    pakdata_t data;
    paktype_t packet = XBEEReceiver.receivePacket(&data);
    if(packet == PAKTYP_RECORD){
        data = lastData;
    }

    if(packet == PAKTYP_FREE || packet == PAKTYP_RECORD) {
        lastPacket = packet;
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
        //menu();
        break;

        case MOD_FREE:
        // free_mod();
        break;

        case MOD_FREE_JOYSTICK:

        break;

        case MOD_RECORD:
        //record();
        break;

        case MOD_PLAY:
        //play_record();
        break;
    }

    if(lastArmPulse - millis() >= 20){
        manageArm();
    }
}
