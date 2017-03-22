#ifndef MSG_H
#define MSG_H

#define HEAD_END '%'

typedef struct {
    char header[4];
} MsgAuth;

typedef struct {
    char mode;
} MsgMode;

typedef struct {
    char separator;
    int value;
} MsgValue;

typedef struct {
    int crc;
    char terminator;
} MsgCRC;

#endif
