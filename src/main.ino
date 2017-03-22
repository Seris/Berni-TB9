#include <Arduino.h>
#include <LiquidCrystal.h>
#include "protocol.h"

LiquidCrystal lcd(8, 9, 2, 3, 5, 6);

XBEEPacketSender XBEESender("RPM");
XBEEPacketReceiver XBEEReceiver("RPM", 15000);

void setup(){
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("En attente...");
}

int i = 0;
int debug = 0;

int g = 0;

void loop(){
    pakdata_t data;
    paktype_t packet = XBEEReceiver.receivePacket(&data);
    char buffer[30];
    memset(buffer, 0, sizeof(buffer));

    if(packet == PAKTYP_RECORD){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Valeur recu:");

        snprintf(buffer, 30, "%.4d %.4d %.4d", data.data1, data.data2, data.data3);
        lcd.setCursor(0, 1);
        lcd.print(buffer);

        XBEESender.sendACK();
    } else if(packet == PAKTYP_BAD){
        XBEESender.sendNACK();
    }

    lcd.setCursor(15, 1);
    lcd.print(g);
    g = !g;
}
