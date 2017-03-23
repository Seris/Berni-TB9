#ifndef ARM_H
#define ARM_H

#include <Arduino.h>

/**
 * Port number of each servomotors and of the clamp
 */
#define SERVO1 45
#define SERVO2 44
#define SERVO3 46
#define CLAMP_CLOSE 47
#define CLAMP_OPEN 48

#define CLAMP_CLOSING 4242

/**
 * Value are in centimeters
 */
#define ARM_MIN_HEIGHT -1.8
#define ARM_JUNC_1 6
#define ARM_JUNC_2 6
#define ARM_JUNC_3 17


typedef struct {
    int16_t fingers;
    int16_t thumb;
    int16_t wrist;
    int time;
} __attribute__((packed)) armrecord_t;

typedef struct {
    double finger_low;
    double finger_high;
    double wrist;
    int16_t thumb;
} armcoord_t;

typedef struct {
    int count;
} armdata_head_t;

void resetArm();
void pulse(int port, int32_t duration);
void setServoAngle(int servo, double angle);
bool validCoordinates(armcoord_t coord);
double getArmHeight(armcoord_t coord);
double dataToAngle(int data);

#endif
