//File Name: Licensed Band User
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
#include <TEST.h>

#define userID 1

// STATE
// int state = 0;                  // 0 = REQUEST
int operation = 0               // 0 = experiment

// -----------------------------------------------------
// |   opCode   |  payload   |     src    |     des    |
// -----------------------------------------------------
// | message[0] | message[1] | message[2] | message[3] |
// -----------------------------------------------------
byte outMessage[4] = { 0 };           // message to be transmit 
byte inMessage[4] = { 0 };           // receive message

// scheduler with 24 slot
// don't send anything from 0:00:00 to 1:00:00, send stuff at ch 1from 1:00:00 to 3:00:00
byte scheduleList[scheduleSize] = {0};

// client 1 and bs knows the session time, client 2 doesn't know.

void synBaseStation() {
    Radio.switchChannel(2);
    while (inMessage[0] != 1 || inMessage[2] != userID) {
        outMessage[0] = lbuStart;
        outMessage[1] = 0;
        outMessage[2] = userID;
        outMessage[3] = userID;
        Radio.sendMessage(lbuSendDuration, outMessage);
    }

    while (true) { // waiting for broad cast
        if (inMessage[0] == bsStart) {
            break;
        }
    }
}

void lbu_process() {
    startTime = millis();
    int time = 0;

    while (true) {
        time = (millis() - startTime) / 60000;
        Radio.switchChannel(scheduleList[time]);
        outMessage[0] = lbuInterrupt;
        outMessage[1] = 0;
        outMessage[2] = userID;
        outMessage[3] = userID;
        Radio.sendMessage(lbuSendDuration, outMessage);
    }
}

void setup()
{
    Serial.begin(9600);

    //  initialize tx rx
    Radio.initialize_trans();
}

void loop(){
    if (operation == 0) {
        synBaseStation();
        lbu_process();
        operation = 2;
    }
    else {

    }
}



