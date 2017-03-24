#include <EEPROM.h>
#include "mod.h"
#include "arm.h"


/**
 * Convert raw data from the glove to proper angle
 * @param  data raw data from the glove
 * @return      proper angles for each part of the arm
 */
armcoord_t convertDataToCoord(pakdata_t data){
    armcoord_t coord;

    coord.finger_low = dataToAngle(lastData.data1 / 2);
    coord.finger_high = dataToAngle(lastData.data1 / 2);

    coord.wrist = dataToAngle(lastData.data3);
    coord.thumb = lastData.data2;

    return coord;
}


/**
 * RECORD Mod of the arm
 */
bool recordStarted = false;
armdata_head_t eepromHead;
int startRecordTime;
int eepromAddr;

/**
 * Init every variables
 */
void initRecording(){
    eepromAddr = sizeof(armdata_head_t);
    eepromHead.count = 0;
    eepromHead.magic = RECORD_MAGIC;
    eepromHead.check = -(eepromHead.count + eepromHead.magic);

    EEPROM.put(0, eepromHead);

    startRecordTime = millis();
    recordStarted = true;
}

/**
 * Commit record into memory by updating the head
 */
void commitRecord(){
    eepromHead.check = -(eepromHead.count + eepromHead.magic);
    EEPROM.put(0, eepromHead);
    recordStarted = false;
}

/**
 * Record coordinates into EEPROM
 * @param  coord coordinate that'll be recorded
 * @return       true on success, false when EEPROM is full
 */
bool recordCoordinates(armcoord_t coord){
    armrecord_t record;
    bool recorded = false;

    if(eepromAddr + sizeof(record) > EEPROM.length()){
        record.fingers = (int16_t) lastData.data1;
        record.thumb = (int16_t) lastData.data2;
        record.wrist = (int16_t) lastData.data3;
        record.time = millis() - startRecordTime;

        EEPROM.put(eepromAddr, record);
        eepromAddr += sizeof(record);
        eepromHead.count++;

        recorded = true;
    }

    return recorded;
}

void modRecord(){
    armcoord_t coord;
    char buffer[16];

    lcd.home();
    lcd.print("Rec mod  EXIT[*]");

    if(!recordStarted){
        initRecording();
    }

    if(lastPacket == PAKTYP_RECORD){
        coord = convertDataToCoord(lastData);

        if(validCoordinates(coord)){
            currentCoordinates = coord;

            lcd.setCursor(0, 1);
            if(recordCoordinates(coord)){
                // lcd.print("Recording... ");
            } else {
                // lcd.print("Memory full !");
            }

            // DEBUG
            lcd.setCursor(0, 1);
            snprintf(buffer, 16, "addr: %d (%d)",
                (int) eepromAddr, eepromHead.count);
            lcd.print(buffer);
        }
    } else if(lastPacket == PAKTYP_FREE){
        operatingMode = MOD_FREE;
        commitRecord();
    }
}



/**
 * FREE Mod of the arm
 */
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
