//File Name: Customer Premise Equipment
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

#define userID 1

// STATE
int state = 0;                  // 0 = REQUEST

// WAIT TIME, PRIME NUMBER x10
byte waitTime[16] = {20,30,50,70,110,130,170,190,230,290,310,370,410,430,470,530};

// -----------------------------------------------------
// |   opCode   |  payload   |     src    |     des    |
// -----------------------------------------------------
// | message[0] | message[1] | message[2] | message[3] |
// -----------------------------------------------------
byte outMessage[4] = { 0 };           // message to be transmit 
byte inMessage[4] = { 0 };           // receive message

// to do list to be done
byte actionList[5] = {0x32, 0x32, 0x32, 0x32, 0x32};

void cpe_process() {
    bool doneFlag = false;
    byte a = 0;
    byte ch = 0;
    Radio.switchChannel(0);

    while (!doneFlag) {
        // every communication done
        if (actionList[5] != 0) {
            doneFlag = true;
        }

        if (state == 0) { // try to request
            outMessage[0] = cpeRequest;
            outMessage[1] = 0;
            outMessage[2] = userID;
            outMessage[3] = actionList[a] << 4;
            Radio.sendMessage(cpeSendDuration, outMessage);
            Radio.receiveMessage(cpeReceiveMaxDuration, inMessage, CPE, userID);
            if (inMessage[0] == bsRespond) {    // receive a bs respond
                Radio.switchChannel(inMessage[1]);
                state = 2; // start sending in assigned channel
            }
            else {                              // receive a bs request
                outMessage[0] = cpeRespond;
                outMessage[1] = inMessage[1];
                outMessage[2] = userID;
                outMessage[3] = inMessage[2];
                Radio.sendMessage(cpeSendDuration, outMessage);
                Radio.switchChannel(inMessage[1]);
                state = 3;
            }
        }
        else if (state == 1) { // send stuff at assgiend channel
            outMessage[0] = cpeSend;
            outMessage[1] = 0;
            outMessage[2] = 0;
            outMessage[3] = 0;
            while (TIME) {
                Radio.sendMessage(cpeSendDuration, outMessage);
                Radio.receiveMessage(cpeReceiveMaxDuration, inMessage);
                if (inMessage[0] == lbuInterrupt) {
                    // store time
                    state == 0;
                    break;
                }
                // TIME++
            }
            if (TIME) { // wrap up
                outMessage[0] = cpeDone;
                outMessage[1] = 0;
                outMessage[2] = 0;
                outMessage[3] = 0;
                while (true) {
                    Radio.sendMessage(cpeSendDuration, outMessage);
                    Radio.receiveMessage(cpeReceiveMaxDuration, inMessage);
                    if (inMessage[0] == cpeAckowledge) {
                        outMessage[0] = cpeClose;
                        outMessage[1] = 0;
                        outMessage[2] = 0;
                        outMessage[3] = 0;
                        Radio.sendMessage(cpeSendDuration, outMessage);
                        Radio.switchChannel(0);
                        actionList[a] = 0;
                        a++;
                        state == 0;
                        break;
                    }
                }
            }
        }
        else if (state == 2) { // listening at assgined channel
            while (true) {
                Radio.receiveMessage(cpeReceiveMaxDuration, inMessage);
                if (inMessage[0] == lbuInterrupt && inMessage[0] == 0) {
                    state == 0;
                    break;
                }
                else if (inMessage[0] == cpeDone) {
                    while (inMessage[0] != cpeClose || TIME) {
                        outMessage[0] = cpeAckowledge;
                        outMessage[1] = 0;
                        outMessage[2] = 0;
                        outMessage[3] = 0;
                        Radio.sendMessage(cpeSendDuration, outMessage);
                        Radio.receiveMessage(cpeReceiveMaxDuration, inMessage);
                    }
                    state == 0;
                    break;
                }
            }
        }
        else {
            //
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

}



