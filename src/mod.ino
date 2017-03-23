#include <EEPROM.h>
#include "mod.h"
#include "arm.h"

bool recordStarted = false;
int startRecordTime;
int eepromAddr;
armdata_head_t eepromHead;

armcoord_t convertDataToCoord(pakdata_t data){
    armcoord_t coord;

    coord.finger_low = dataToAngle(lastData.data1 / 2);
    coord.finger_high = dataToAngle(lastData.data1 / 2);

    coord.wrist = dataToAngle(lastData.data3);
    coord.thumb = lastData.data2;

    return coord;
}

void modFree(){
    armcoord_t coord;
    char buffer[16];

    lcd.home();
    lcd.print("Free mod EXIT[*]");

    recordStarted = false;

    if(lastPacket == PAKTYP_FREE){
        coord = convertDataToCoord(lastData);
        if(validCoordinates(coord)){
            currentCoordinates = coord;
        }
    } else if(lastPacket == PAKTYP_RECORD){
        operatingMode = MOD_RECORD;
    }

    lcd.setCursor(0, 1);
    snprintf(buffer, 16, "coord: %.2d %.2d %.2d",
        (int) currentCoordinates.wrist,
        (int) currentCoordinates.finger_low,
        (int) currentCoordinates.finger_high);
    lcd.print(buffer);
}

void modRecord(){
    armrecord_t record;
    armcoord_t coord;
    char buffer[16];

    lcd.home();
    lcd.print("Rec mod  EXIT[*]");

    if(!recordStarted){
        startRecordTime = millis();
        eepromAddr = sizeof(armdata_head_t);
        eepromHead.count = 0;
        EEPROM.put(0, eepromHead);
        recordStarted = true;
    }

    if(lastPacket == PAKTYP_RECORD){
        coord = convertDataToCoord(lastData);
        lcd.setCursor(0, 1);
        snprintf(buffer, 16, "coord: %.2d %.2d %.2d",
            (int) currentCoordinates.wrist,
            (int) currentCoordinates.finger_low,
            (int) currentCoordinates.finger_high);
        lcd.print(buffer);

        lcd.setCursor(0, 1);
        if(validCoordinates(coord)){
            currentCoordinates = coord;

            lcd.setCursor(0, 1);
            if(eepromAddr + sizeof(record) > EEPROM.length()){
                lcd.print("Recording...");

                record.fingers = (int16_t) lastData.data1;
                record.thumb = (int16_t) lastData.data2;
                record.wrist = (int16_t) lastData.data3;
                record.time = millis() - startRecordTime;

                EEPROM.put(eepromAddr, record);
                eepromAddr += sizeof(record);
                eepromHead.count++;
            } else {
                lcd.print("Memory full");
            }

            lcd.setCursor(0, 1);
            snprintf(buffer, 16, "addr: %d (%d)",
                (int) eepromAddr, eepromHead.count);
            lcd.print(buffer);
        }
    } else if(lastPacket == PAKTYP_FREE){
        operatingMode = MOD_FREE;
    }
}


int lastJoystickPull = 0;
void modJoystick(){
    double x, y;
    armcoord_t coord = currentCoordinates;
    char buffer[16];

    if(millis() - lastJoystickPull > 200){
        x = (double) analogRead(JOYSTICK_X);
        y = (double) analogRead(JOYSTICK_Y);

        if(x > JOYSTICK_X0_MIN && x < JOYSTICK_X0_MAX){
            x = 0;
        } else {
            x = (x - JOYSTICK_X0_MIN) / 50;
        }

        if(y > JOYSTICK_Y0_MIN && y < JOYSTICK_Y0_MAX){
            y = 0;
        } else {
            y = (y - JOYSTICK_Y0_MIN) / 50;
        }

        lcd.home();
        coord.wrist += x;
        snprintf(buffer, 16, "%.2d ", (int) coord.wrist);
        lcd.print(buffer);
        if(validCoordinates(coord)){
            lcd.print("valid  ");
            currentCoordinates = coord;
        } else {
            lcd.print("invalid");
        }

        lastJoystickPull = millis();
    }
}
