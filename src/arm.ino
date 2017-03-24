#include <Arduino.h>
#include "arm.h"


int clampStatus = CLAMP_OPEN;
uint32_t clampStartMoving = 0;

/**
 * Send pulse to arm
 */
void manageArm(){
    setServoAngle(SERVO1, currentCoordinates.wrist);
    setServoAngle(SERVO2, currentCoordinates.finger_low);
    setServoAngle(SERVO3, currentCoordinates.finger_high);

    if(currentCoordinates.thumb > 520 && clampStatus == CLAMP_OPEN){
        digitalWrite(CLAMP_CLOSE, HIGH);
        digitalWrite(CLAMP_OPEN, LOW);
        clampStartMoving = millis();
        clampStatus = CLAMP_CLOSE;
    } else if(currentCoordinates.thumb <= 520 && clampStatus == CLAMP_CLOSE) {
        digitalWrite(CLAMP_OPEN, HIGH);
        digitalWrite(CLAMP_CLOSE, LOW);
        clampStartMoving = millis();
        clampStatus = CLAMP_OPEN;
    }

    if(millis() - clampStartMoving > 1100){
        digitalWrite(CLAMP_CLOSE, LOW);
        digitalWrite(CLAMP_OPEN, LOW);
    }
}



/**
 * Convert data value to angle
 */
double dataToAngle(int data){
    return ((double) data) / 1024 * 90;
}

/**
 * Set angle of servomotor
 * @param servo
 * @param angle in degree
 */
void setServoAngle(int servo, double angle){
    int32_t duration = (int32_t) (1500 - angle / 90 * 1000);
    if(duration > 1500)
        duration = 1500;
    else if(duration < 500)
        duration = 500;

    pulse(servo, duration);
}

/**
 * Send a pulse to a servomotor
 * @param port     Arduino Mega Digital Port
 * @param duration Duration of the pulse
 */
void pulse(int port, int32_t duration){
    digitalWrite(port, HIGH);
    delayMicroseconds(duration);
    digitalWrite(port, LOW);
}

/**
 * Setup arm at beggining
 */
void resetArm(){
    DDRL = 0xFF;
    for(int i = 0; i < 40; i++){
        PORTL = 0x38;
        delayMicroseconds(1500);
        PORTL = 0x00;
        delay(20);
    }

    currentCoordinates = {0, 0, 0, 0};

    digitalWrite(CLAMP_OPEN, HIGH);
    delay(1300);
    digitalWrite(CLAMP_OPEN, LOW);
    clampStatus = CLAMP_OPEN;
    clampStartMoving = 0;
}

/**
 * Check if coordinates are valid
 * @param  coord [description]
 * @return       [description]
 */
bool validCoordinates(armcoord_t coord){
    return getArmHeight(coord) > ARM_MIN_HEIGHT
        && coord.finger_low >= 0 && coord.finger_low <= 90
        && coord.finger_high >= 0 && coord.finger_high <= 90
        && coord.wrist >= 0 && coord.wrist <= 90
        && coord.thumb >= 0 && coord.thumb <= 1024;
}

/**
 * Get the arm height in function of the relative angle of each part
 * @param  serv1_ang angle in degree
 * @param  serv2_ang angle in degree
 * @param  serv3_ang angle in degree
 * @return           the height in cm of the arm
 */
double getArmHeight(armcoord_t coord){
    return
        ARM_JUNC_1 * cos(coord.wrist / 180 * M_PI) +
        ARM_JUNC_2 * cos((coord.wrist  + coord.finger_low) / 180 * M_PI) +
        ARM_JUNC_3 * cos((coord.wrist  + coord.finger_low + coord.finger_high) / 180 * M_PI);
}
