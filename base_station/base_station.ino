//File Name: Base Station
//Project Name: Cogntiive Radio Network Enviroment Deployment
//Engineer: Kevin Yu
//Date: 3-22-2021
//Description: 
//New transmitting and receiving method applied. (method checks the Rx Fifo for any data it contains)
//It allows you to do several things in a loop.
//In addition, the gdo0 and gdo2 pin are not required.
//https://github.com/LSatan/SmartRC-CC1101-Driver-Lib
//by Little_S@tan
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <RADIO.h>

//STATE
int state = 0;                  // 0 = IDEL


byte outMessage[4] = {0};           // message to be transmit 
int ch = 0;                     // channel number declaration and initialization
byte inMessage[4] = {0};           // receive message


void bs_process() {
    while (true) {
        if (state == 0) { // try to receive request
            Radio.receiveMessage(receiveMaxDuration, inMessage);
            if (inMessage[0] == 0) {
                //no message received
            }
            else if (inMessage[0] == cpeRequest) {
                //receive request
                outMessage[0] = bsRespond;
                outMessage[1] = 0;
                outMessage[2] = inMessage[2];
                outMessage[3] = inMessage[3];
                Radio.sendMessage(sendDuration, outMessage);
                state = 1;
            }
            else {
                //
            }
        }
        else if (state == 1) { // try to receive response
            Radio.receiveMessage(receiveMaxDuration, inMessage);
            if (inMessage[0] == 0) {
                //no message received
            }
            else if (inMessage[0] == cpeRespond) {
                //receive response
                outMessage[0] = bsRespond;
                outMessage[1] = 0;
                outMessage[2] = inMessage[2];
                outMessage[3] = inMessage[3];
                Radio.sendMessage(sendDuration, outMessage);
                state = 0;
            }
            else {
                //
            }
        }
        else if (state == 2) {

        }
        else {

        }
    }
}

void setup()
{
    Serial.begin(9600);

    //  initialize tx rx
    Radio.initialize_trans();
}

void loop() {
    Serial.println("Let's go");
    //outMessage[0] = bsRespond;
    //outMessage[1] = 0;
    //outMessage[2] = 3;
    //outMessage[3] = 4;
    //Radio.sendMessage(sendDuration, outMessage);
    Radio.receiveMessage(receiveMaxDuration, inMessage);
    Serial.println(inMessage[0]);
    Serial.println(inMessage[1]);
    Serial.println(inMessage[2]);
    Serial.println(inMessage[3]);

    Serial.println();
}
