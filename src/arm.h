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

/**
 * Value are in centimeters
 */
#define ARM_MIN_HEIGHT -2
#define ARM_JUNC_1 6
#define ARM_JUNC_2 6
#define ARM_JUNC_3 17

/**
 * Port number of the joystick
 */
#define JOYSTICK_X 7
#define JOYSTICK_Y 6

/**
 * Joystick Values
 */
#define JOYSTICK_XMIN 200
#define JOYSTICK_XMAX 900
#define JOYSTICK_YMIN 200
#define JOYSTICK_YMAX 900
#define JOYSTICK_X0_MIN 583
#define JOYSTICK_X0_MAX 592
#define JOYSTICK_Y0_MIN 538
#define JOYSTICK_Y0_MAX 545


typedef enum {
    MOD_MENU,
    MOD_FREE,
    MOD_FREE_GLOVE,
    MOD_PLAY,
    MOD_RECORD
} opmod_t;

typedef struct {
    int16_t fingers;
    int16_t thumb;
    int16_t wrist;
} __attribute__((packed)) armcoord_t;

typedef struct {
    armcoord_t coord;
    int time;
} __attribute__((packed)) armdata_t;

typedef struct {
    int count;
} armdata_head_t;

void resetArm();
void pulse(int port, int32_t duration);
void setServoAngle(int servo, double angle);
double getArmHeight(double serv1_ang, double serv2_ang, double serv3_ang);

#endif
