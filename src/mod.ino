#include <EEPROM.h>
#include "mod.h"
#include "arm.h"

void printCurrentCoordinates(){
    char buffer[16];
    char* clamp;
    if (clampStatus == CLAMP_CLOSE) {
        clamp = "Close";
    } else {
        clamp = "Open ";
    }

    snprintf(buffer, 16, "%.2d %.2d %.2d %s",
        (int) currentCoordinates.wrist,
        (int) currentCoordinates.finger_low,
        (int) currentCoordinates.finger_high,
        clamp);
    lcd.print(buffer);
}


/**
 * Convert raw data from the glove to proper angle
 * @param  data raw data from the glove
 * @return      proper angles for each part of the arm
 */
armcoord_t convertDataToCoord(pakdata_t data){
    armcoord_t coord;
    char buffer[16];

    coord.finger_low = dataToAngle(data.data1 / 2);
    coord.finger_high = dataToAngle(data.data1 / 2);

    coord.wrist = dataToAngle(data.data3);
    coord.thumb = data.data2;

    return coord;
}


/**
 * RECORD Mod of the arm
 */
bool recordStarted = false;
armdata_head_t eepromHead;
int startRecordTime;
int eepromAddrRecord;

/**
 * Init every variables
 */
void initRecording(){
    eepromAddrRecord = sizeof(armdata_head_t);
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

    if(eepromAddrRecord + sizeof(record) < EEPROM.length()){
        record.fingers = (int16_t) lastData.data1;
        record.thumb = (int16_t) lastData.data2;
        record.wrist = (int16_t) lastData.data3;
        record.time = millis() - startRecordTime;

        EEPROM.put(eepromAddrRecord, record);
        eepromAddrRecord += sizeof(record);
        eepromHead.count++;

        recorded = true;
    }

    return recorded;
}

void modRecord(){
    armcoord_t coord;

    lcd.home();
    lcd.print("Rec. SAV&EXIT[*]");

    if(!recordStarted){
        initRecording();
    }

    if(lastPacket == PAKTYP_RECORD){
        coord = convertDataToCoord(lastData);

        if(validCoordinates(coord)){
            currentCoordinates = coord;

            lcd.setCursor(0, 1);
            if(recordCoordinates(coord)){
                lcd.print("Recording... ");
            } else {
                lcd.print("Memory full !");
            }
        }
    } else if(lastPacket == PAKTYP_FREE){
        commitRecord();
        operatingMode = MOD_FREE;
    }

    if(keypad.getKey() == EXIT_CHAR){
        lcd.clear();
        lcd.home();
        lcd.print("Saving & Exiting");

        commitRecord();
        resetArm();
        lcd.clear();

        operatingMode = MOD_MENU;
    }
}



/**
 * PLAY Mod of the arm
 */
bool playingStarted = false;
int playIndex;
armdata_head_t recordHead;
armrecord_t currentRecord;
uint32_t playTimeOrigin;

/**
 * Init all variables to record
 * @return true if we have data to play, false if not
 */
bool initPlaying(){
    EEPROM.get(0, recordHead);

    if(recordHead.count + recordHead.magic + recordHead.check == 0
        && recordHead.count > 0){

        playingStarted = true;
        playIndex = 0;
        playTimeOrigin = millis();

        EEPROM.get(sizeof(armdata_head_t), currentRecord);

        return true;
    }

    return false;
}

/**
 * Return the "index'th" record stored in EEPROM memory
 * @param  index [description]
 * @return       [description]
 */
armrecord_t getRecord(int index){
    armrecord_t record;
    EEPROM.get(sizeof(armdata_head_t) + sizeof(armrecord_t) * playIndex, record);
    return record;
}

/**
 * Update coordinates in function of a given record
 * @param record
 */
void playRecord(armrecord_t record){
    pakdata_t data;
    armcoord_t coord;

    data.data1 = (int) record.fingers;
    data.data2 = (int) record.thumb;
    data.data3 = (int) record.wrist;

    coord = convertDataToCoord(data);
    if(validCoordinates(coord)){
        currentCoordinates = coord;
    }
}

void modPlay(){
    lcd.home();
    lcd.print("Play mod EXIT[*]");
    lcd.setCursor(0, 1);

    if(!playingStarted){
        if(!initPlaying()){
            lcd.print("No data available");
        }
    } else {
        if(millis() - playTimeOrigin > currentRecord.time){
            playRecord(currentRecord);

            playIndex++;
            if(playIndex >= recordHead.count){
                initPlaying();
                delay(300);
            }

            currentRecord = getRecord(playIndex);

            lcd.setCursor(0, 1);
            printCurrentCoordinates();
        }
    }

    if(keypad.getKey() == EXIT_CHAR){
        lcd.clear();
        lcd.home();
        lcd.print("Exiting...");

        resetArm();
        lcd.clear();

        playingStarted = false;
        operatingMode = MOD_MENU;
    }
}



/**
 * FREE Mod of the arm
 */
void modFree(){
    armcoord_t coord;

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
    printCurrentCoordinates();

    if(keypad.getKey() == EXIT_CHAR){
        lcd.clear();
        lcd.home();

        lcd.print("Exiting...");
        resetArm();

        lcd.clear();

        operatingMode = MOD_MENU;
    }
}


/**
 * Display menu and manager keyboard
 */
void modMenu(){
    lcd.home();
    lcd.print("1.Glove 2.Play ");
    lcd.setCursor(0, 1);
    lcd.print("3.Reset 4.Joyst.");

    char key = keypad.getKey();
    if(key == '1'){
        operatingMode = MOD_FREE;
        lcd.clear();
    } else if(key == '2') {
        operatingMode = MOD_PLAY;
        lcd.clear();
    } else if(key == '3'){
        lcd.clear();
        lcd.home();
        lcd.print("Please wait..");
        resetArm();

        lcd.clear();
    } else if(key == '4'){
        operatingMode = MOD_JOYSTICK;
        lcd.clear();
    }
}



uint32_t lastJoystickPull = 0;
void modJoystick(){
    armcoord_t coord = currentCoordinates;
    char key;
    double x, y;

    lcd.home();
    lcd.print("Joystick EXIT[*]");
    lcd.setCursor(0, 1);
    printCurrentCoordinates();

    if(millis() - lastJoystickPull > 100){
        x = (double) analogRead(JOYSTICK_X);
        y = (double) analogRead(JOYSTICK_Y);

        if(x < 400){
            coord.wrist -= 3;
        } else if(x > 700){
            coord.wrist += 3;
        }

        if(y < 400){
            coord.finger_low += 1.5;
        } else if(y > 700){
            coord.finger_low -= 1.5;
        }

        if(coord.wrist < -90 ) coord.wrist = -90;
        else if (coord.wrist > 90) coord.wrist = 90;

        if(coord.finger_low < -90 ) coord.finger_low = -90;
        else if (coord.finger_low > 90) coord.finger_low = 90;

        coord.finger_high = coord.finger_low;

        if(validCoordinates(coord)){
            currentCoordinates = coord;
        }

        lastJoystickPull = millis();
    }

    key = keypad.getKey();
    if(key == '1'){
        currentCoordinates.thumb = 0;
    } else if(key == '3'){
        currentCoordinates.thumb = 1024;
    } else if(key == EXIT_CHAR){
        lcd.clear();
        lcd.home();
        lcd.print("Exiting...");
        resetArm();

        operatingMode = MOD_MENU;
        lcd.clear();
    }
}
