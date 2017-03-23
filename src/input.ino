#include <Keypad.h>

#define KP_ROWS 4
#define KP_COLS 3

char keypad_keys[KP_ROWS][KP_COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'#', '0', '*'}
};

byte rowPins[KP_ROWS] = {25, 26, 27, 28};
byte colPins[KP_COLS] = {22, 23, 24};

Keypad keypad(makeKeymap(keypad_keys), rowPins, colPins, KP_ROWS, KP_COLS);
