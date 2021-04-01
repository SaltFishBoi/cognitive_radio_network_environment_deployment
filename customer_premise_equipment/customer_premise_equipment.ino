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
#include <TEST.h>

#define userID 1

// STATE
int state = 0;                  // 0 = REQUEST
int operation = 0               // 0 = experiment, 1 = output report

// TIME
int startTime = 0;              // starting reference time

// DATA
int interruptNum = 0;

// -----------------------------------------------------
// |   opCode   |  payload   |     src    |     des    |
// -----------------------------------------------------
// | message[0] | message[1] | message[2] | message[3] |
// -----------------------------------------------------
byte outMessage[4] = { 0 };           // message to be transmit 
byte inMessage[4] = { 0 };           // receive message

// to do list to be done
// talk to clientID 3 for 2 hrs (2 min)
byte actionList[actionSize] = {0};

// client 1 and bs knows the session time, client 2 doesn't know.

void connectBaseStation() {
    Radio.switchChannel(1);
    while (inMessage[0] != 1 || inMessage[2] != userID) {
        outMessage[0] = cpeStart;
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

void cpe_process() {
    bool doneFlag = false;   // done with all actions
    bool sendFinishFlag; // finish an action, one session
    startTime = millis();
    byte a = 0;
    byte ch = 0;
    int start_time;
    int mem_location = 0;
    byte e = 0;
    byte t = 0; // program run time in min
    byte interrupt = 0;
    Radio.switchChannel(0);
 

    while (!doneFlag) {
        // every communication done
        if (actionList[5] != 0) {
            doneFlag = true;
        }

        if ((millis() - startTime) / 1000 / 60 == t) {// record
            TEST.record(mem_location, t, interrupt);
            interrupt = 0;
            mem_location+=2;
            t++;
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
                state = 1; // start sending in assigned channel
                sendFinishFlag = false;
                start_time = millis();  // start reference time
            }
            else {                              // receive a bs request
                outMessage[0] = cpeRespond;
                outMessage[1] = inMessage[1];
                outMessage[2] = userID;
                outMessage[3] = inMessage[2];
                Radio.sendMessage(cpeSendDuration, outMessage);
                Radio.switchChannel(inMessage[1]);
                state = 2;
            }
        }
        else if (state == 1) { // send stuff at assgiend channel
            outMessage[0] = cpeSend;
            outMessage[1] = 0;
            outMessage[2] = 0;
            outMessage[3] = 0;

            Radio.receiveMessage(cpeReceiveMaxDuration, inMessage);
            if ((millis() - start_time) >= (actionList[a] & 0x0F) * 60 * 1000) { // compare them in milli-second
                Radio.sendMessage(cpeSendDuration, outMessage);
            }
            else {
                sendFinishFlag = true;
            }

            if (inMessage[0] == lbuInterrupt) {
                actionsList[a] = actionsList[a] - ((millis() - start_time) / 60000); // convert milli-second into minutes, subtract the finished time
                interrupt++;
                state == 0;
            }

            if (sendFinishFlag) {           // if finished, no interrupt during the sending, wrap up
                outMessage[0] = cpeDone;
                outMessage[1] = 0;
                outMessage[2] = 0;
                outMessage[3] = 0;

                Radio.sendMessage(cpeSendDuration, outMessage);
                Radio.receiveMessage(cpeReceiveMaxDuration, inMessage);
                if (inMessage[0] == cpeAckowledge) {
                    outMessage[0] = cpeClose;
                    outMessage[1] = 0;
                    outMessage[2] = 0;
                    outMessage[3] = 0;
                    Radio.sendMessage(cpeSendDuration, outMessage);
                    Radio.switchChannel(0);                             // switch back to default ch to setup session 
                    actionList[a] = 0;  
                    a++;
                    state == 0;
                    break;
                }
            }
        }
        else if (state == 2) { // listening at assgined channel
            Radio.receiveMessage(cpeReceiveMaxDuration, inMessage);
            if (inMessage[0] == lbuInterrupt && inMessage[0] == 0) {
                state == 0;
                break;
            }
            else if (inMessage[0] == cpeDone) {
                e = 0;
            }
            else if (inMessage[0] != cpeClose && e != receiveExpired) {
                outMessage[0] = cpeAckowledge;
                outMessage[1] = 0;
                outMessage[2] = 0;
                outMessage[3] = 0;
                Radio.sendMessage(cpeSendDuration, outMessage);
                Radio.receiveMessage(cpeReceiveMaxDuration, inMessage);
                e++;
            }
            else if (e == receiveExpired) {
                Radio.switchChannel(0);             // switch back to default ch to setup session
                state == 0;
                break;
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

    if (operation == 0) {
        connectBaseStation();
        cpe_process();
        operation = 2;
    }
    else if (operation == 1) {
        TEST.report();
        operation = 2;
    }
    else {

    }
}



