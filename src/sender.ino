#include <Arduino.h>
#include "protocol.h"

XBEEPacketSender::XBEEPacketSender(const char* header) {
    this->header = header;
}

/**
 * Send ACK packet through XBEE module
 */
void XBEEPacketSender::sendACK(){
    this->sendHeader(PAKTYP_ACK);
    this->sendCRC(PAKTYP_ACK);
}

/**
 * Send NACK packet through XBEE module
 */
void XBEEPacketSender::sendNACK(){
    this->sendHeader(PAKTYP_NACK);
    this->sendCRC(PAKTYP_NACK);
}

/**
 * Send PLAY packet through XBEE module to set the operating mode of the
 * arm to Play
 */
void XBEEPacketSender::sendPlay(){
    this->sendHeader(PAKTYP_PLAY);
    this->sendCRC(PAKTYP_PLAY);
}

/**
 * Send RECORD packet through XBEE module to set the operating mode of the
 * arm to Record along with data
 */
void XBEEPacketSender::sendRecord(int data1, int data2, int data3){
    this->sendHeader(PAKTYP_RECORD);
    this->sendData(data1);
    this->sendData(data2);
    this->sendData(data3);
    this->sendCRC(data1 + data2 + data3 + PAKTYP_RECORD);
}


void XBEEPacketSender::sendHeader(paktype_t type){
    Serial.write(this->header);
    Serial.write(HEAD_SEP);
    Serial.write(type);
}

void XBEEPacketSender::sendData(int data){
    char buffer[5];
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, 5, "%.4d", data);
    Serial.write(DATA_START);
    Serial.write(buffer);
}

void XBEEPacketSender::sendCRC(char crc){
    Serial.write(CRC_START);
    Serial.write(crc);
    Serial.write(PACKET_END);
}
