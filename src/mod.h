#ifndef MOD_H
#define MOD_H

#include <Arduino.h>

#define RECORD_MAGIC 0x42424242

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
#define JOYSTICK_X0_MIN 500
#define JOYSTICK_X0_MAX 700
#define JOYSTICK_Y0_MIN 500
#define JOYSTICK_Y0_MAX 700

/**
 * Operating mods
 */
typedef enum {
    MOD_MENU,
    MOD_FREE,
    MOD_JOYSTICK,
    MOD_PLAY,
    MOD_RECORD
} opmod_t;

typedef struct {
    int16_t fingers;
    int16_t thumb;
    int16_t wrist;
    int time;
} __attribute__((packed)) armrecord_t;

typedef struct {
    int count;
    int32_t magic;
    int32_t check;
} armdata_head_t;

void modJoystick();
void modFree();

#endif
