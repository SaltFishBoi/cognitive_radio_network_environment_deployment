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

#define LED1 3
#define LED2 4

#define userID 0

// TIME
unsigned long startTime = 0;              // starting reference time

// STATE
// int state = 0;                  // 0 = REQUEST
int operation = 0;               // 0 = experiment

// -----------------------------------------------------
// |   opCode   |  payload   |     src    |     des    |
// -----------------------------------------------------
// | message[0] | message[1] | message[2] | message[3] |
// -----------------------------------------------------
byte outMessage[4] = { 0 };           // message to be transmit 
byte inMessage[4] = { 0 };           // receive message

// scheduler with 24 slot
// don't send anything from 0:00:00 to 1:00:00, send stuff at ch1 from 1:00:00 to 3:00:00
byte scheduleList[scheduleSize] = {0};

// client 1 and bs knows the session time, client 2 doesn't know.

void synBaseStation() {
    Radio.switchChannel(2);
    outMessage[0] = lbuStart;
    outMessage[1] = 0;
    outMessage[2] = userID;
    outMessage[3] = userID;
    Radio.sendMessage(shortSendDuration, outMessage);
    while (true) {
        Radio.receiveMessage(longReceiveMaxDuration, inMessage, LBU, userID);
        if (inMessage[0] == bsAcknowledge && inMessage[2] == userID){
             break;
        }
    }
    Serial.println("Connect Base station");
    // this blink indicate success connect to base station
    digitalWrite(LED1, LOW); // LED1 blink indicate ready
    delay(500);
    digitalWrite(LED1, HIGH);

    while (true) { // waiting for broad cast
        Radio.receiveMessage(longReceiveMaxDuration, inMessage, LBU, userID);
        if (inMessage[0] == bsStart) {
            break;
        }
    }
}

void lbu_process() {
    unsigned long t = 0;
    outMessage[0] = lbuInterrupt;
    outMessage[1] = 0;
    outMessage[2] = userID;
    outMessage[3] = userID;
    while (true) {
        t = (millis() - startTime) / (1000*secDiv);
        if (((scheduleList[t%scheduleSize] & 0xF0) >> 4) != 0)
        {
            Radio.switchChannel((scheduleList[t%scheduleSize] & 0xF0) >> 4);
            Radio.sendMessage(shortSendDuration, outMessage);
            //Serial.println((scheduleList[t%scheduleSize] & 0xF0) >> 4);
        }
        if((scheduleList[t%scheduleSize] & 0x0F) != 0)
        {
            Radio.switchChannel(scheduleList[t%scheduleSize] & 0x0F);
            Radio.sendMessage(shortSendDuration, outMessage);
            //Serial.println(scheduleList[t%scheduleSize] & 0x0F);
        }
        
        //delay(500);
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(LED1,OUTPUT); // initialize leds
    pinMode(LED2,OUTPUT);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    //  initialize tx rx
    Radio.initialize_trans();
}

void loop(){
    if (operation == 0) {
        Test.initialize_scheduleList(userID, scheduleList);
        digitalWrite(LED1, HIGH); // blue lighton indicate on operation 0
        synBaseStation();
        digitalWrite(LED1, LOW); // blue lightout indicate finish syn and start process
        
        lbu_process();

        // so far, below code never run since infinite loop in lbu_process 
        for(int i = 0; i < scheduleSize; i++)
        {
            Serial.println(scheduleList[i]);
        }
        operation = 2;
    }
    else if (operation == 1) {
        // debug
    }
    else {
        ;
    }
}
