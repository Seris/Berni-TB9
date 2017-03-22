#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>

#define HEAD_SEP '%'
#define DATA_START '$'
#define CRC_START '&'
#define PACKET_END "$$"

#define RECV_BUF_SIZE 10

#define SERIAL_TIMEOUT 3
#define RECV_DEFAULT_TIMEOUT 15000

/**
 * Packet:
 * (HEADER)%(TYPE)[$(D1)][$(D2)]&(CRC)$$
 * CRC = Data Sum + TYPE
 */

typedef enum {
    PAKTYP_NONE,
    PAKTYP_BAD,
    PAKTYP_PLAY = '*',
    PAKTYP_RECORD = '#',
    PAKTYP_NACK = '?',
    PAKTYP_ACK = '!'
} paktype_t;

typedef struct {
    int data1;
    int data2;
    int data3;
} pakdata_t;

typedef struct {
    char start;
    int crc;
    char end[2];
} __attribute__((packed)) crc_t;


/**
 * XBEEPacketSender
 * Used to send packet through the XBEE module
 */
class XBEEPacketSender {
    void sendHeader(paktype_t type);
    void sendData(int data1);
    void sendCRC(char crc);

    const char* header;

public:
    XBEEPacketSender(const char* header);

    void sendACK();
    void sendNACK();
    void sendPlay();
    void sendRecord(int data1, int data2, int data3);
};


/**
 * XBEEPacketReceiver
 * Used to receive packet from the XBEE module
 */
class XBEEPacketReceiver {
    const char* header;
    int headerSize;
    char buffer[RECV_BUF_SIZE];
    bool validCRC(char crc);
    uint32_t timeout;

public:
    XBEEPacketReceiver(const char* header, uint32_t timeout);
    int checkForHeader();
    bool getData(int* data);
    paktype_t receivePacket(pakdata_t* data);
    bool checkTimeout(uint32_t startTime);
};

#endif
