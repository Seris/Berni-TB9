#include <Arduino.h>
#include "protocol.h"

XBEEPacketReceiver::XBEEPacketReceiver(const char* header, uint32_t timeout){
    this->header = header;
    this->headerSize = strlen(header) + 1;
    this->timeout = timeout;
    memset(this->buffer, 0, RECV_BUF_SIZE);
    Serial.setTimeout(SERIAL_TIMEOUT);
}


paktype_t XBEEPacketReceiver::receivePacket(pakdata_t* data){
    char rawtype = 0;
    paktype_t type = PAKTYP_NONE;

    if(this->checkForHeader()){
        if(Serial.readBytes(&rawtype, 1) == 1){
            switch((paktype_t) rawtype){
                case PAKTYP_ACK:
                case PAKTYP_NACK:
                case PAKTYP_PLAY:
                if(this->validCRC(rawtype)){
                    type = rawtype;
                } else {
                    type = PAKTYP_BAD;
                }
                break;

                case PAKTYP_RECORD:
                if(this->getData(&data->data1)
                    && this->getData(&data->data2)
                    && this->getData(&data->data3)
                    && this->validCRC(data->data1 + data->data2 + data->data3 + PAKTYP_RECORD)){
                    type = PAKTYP_RECORD;
                }
                break;

                default:
                type = PAKTYP_BAD;
            }
        } else {
            type = PAKTYP_BAD;
        }
    }

    return type;
}


bool XBEEPacketReceiver::getData(int* data){
    int j;
    bool received = false;
    char buffer[6];
    memset(buffer, 0, sizeof(buffer));

    if(Serial.readBytes(buffer, 5*sizeof(char)) == 5){
        if(buffer[0] == '$'){
            for(j = 1; j < 5 && buffer[j] == '0'; j++);
            *data = strtol(&buffer[j], NULL, 10);
            received = true;
        }
    }

    return received;
}


bool XBEEPacketReceiver::validCRC(char crc){
    bool valid = false;
    char buffer[5];
    memset(buffer, 0, 5);

    if(Serial.readBytes(buffer, 4) == 4){
        valid = (buffer[0] == CRC_START
                    && buffer[1] == crc
                    && strncmp(&buffer[2], PACKET_END, 2) == 0);
    }

    return valid;
}

int XBEEPacketReceiver::checkForHeader(){
    int headerReceived = 0;
    char c;
    uint32_t startTime = micros();

    while(headerReceived == 0
        && this->checkTimeout(startTime - 1000)
        && Serial.readBytes(&c, 1) > 0){
        for(int i = 1; i < this->headerSize; i++){
            this->buffer[i-1] = this->buffer[i];
        }

        this->buffer[this->headerSize-1] = c;

        if(c == HEAD_SEP && strncmp(this->buffer, this->header, this->headerSize-1) == 0){
            headerReceived = 1;
            memset(this->buffer, 0, sizeof(this->buffer));
        }
    }

    return headerReceived;
}

bool XBEEPacketReceiver::checkTimeout(uint32_t startTime){
    uint32_t duration = (uint32_t) (micros() - startTime);
    return duration < this->timeout;
}
