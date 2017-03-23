#include <Arduino.h>
#include "arm.h"

/**
 * Convert data value to angle
 */
void dataToAngle(){

}

/**
 * Send pulse to arm
 */
void manageArm(){
    
}


/**
 * Set angle of servomotor
 * @param servo
 * @param angle in degree
 */
void setServoAngle(int servo, double angle){
    int32_t duration = 1500 - angle / 90 * 1000;
    if(duration > 1500) duration = 1500;
    else if(duration < 500) duration = 500;
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
}

/**
 * Get the arm height in function of the relative angle of each part
 * @param  serv1_ang angle in degree
 * @param  serv2_ang angle in degree
 * @param  serv3_ang angle in degree
 * @return           the height in cm of the arm
 */
double getArmHeight(double serv1_ang, double serv2_ang, double serv3_ang){
    return
        ARM_JUNC_1 * cos(serv1_ang / 180 * M_PI) +
        ARM_JUNC_2 * cos((serv1_ang  + serv2_ang) / 180 * M_PI) +
        ARM_JUNC_3 * cos((serv1_ang  + serv2_ang  + serv3_ang) / 180 * M_PI);
}
