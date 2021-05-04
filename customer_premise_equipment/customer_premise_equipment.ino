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

#define LED1 3
#define LED2 4

#define userID 3

// STATE
int state = 0;                  // 0 = REQUEST
int operation = 0;               // 0 = experiment, 1 = output report

// TIME
unsigned long startTime = 0;              // starting reference time

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
// talk to clientID 3 for 2 hrs 
// byte actionList[actionSize] = {0};

// 5/1/21 *update* default communication time (session time) 1 time division (2hr)
// talk to other client round robin
byte requestTarget = userID+1; // request the next target

// client 1 and bs knows the session time, client 2 doesn't know.

void connectBaseStation() {
    Radio.switchChannel(1);
    outMessage[0] = cpeStart;
    outMessage[1] = 0;
    outMessage[2] = userID;
    outMessage[3] = userID;
    Radio.sendMessage(shortSendDuration, outMessage);
    while (true) {
        Radio.receiveMessage(longReceiveMaxDuration, inMessage, ANY, userID); // non CPE type listen
        if (inMessage[0] == bsAcknowledge && inMessage[2] == userID){
             inMessage[0] = 0; // clear message
             break;
        }
    }
    Serial.println("Connect Base station");
    digitalWrite(LED1, LOW); // LED1 blink indicate ready
    delay(500);
    digitalWrite(LED1, HIGH);

    Radio.switchChannel(2);
    
    while (true) { // waiting for broad cast
        Radio.receiveMessage(longReceiveMaxDuration, inMessage, ANY, userID); // non CPE type listen
        if (inMessage[0] == bsStart) {
            break;
        }
    }
    Serial.println("Connect heard");
}

void cpe_process() {
    bool doneFlag = false;   // done with all actions
    bool sendFinishFlag; // finish an action, one session
    startTime = millis();
    byte ch = 0;
    unsigned long ref_time;
    int mem_location = 0;
    byte e = 0;
    byte w = 0; // waitTime index
    byte t = 0; // program run time in time div
    byte interrupt = 0;
    Radio.switchChannel(0);
    outMessage[0] = cpeRequest;
    outMessage[1] = 0;
    outMessage[2] = userID;

    while (requestTarget%numClient == 0 || requestTarget%numClient == userID){
        requestTarget++;
    }
    outMessage[3] = requestTarget%numClient; // translate to client ID
    
    while (!doneFlag) {
        // every communication done
        if ((millis() - startTime) / (1000*secDiv*scheduleSize) == endInDay) {
            doneFlag = true;
            //Serial.println("finish All");
        }
        if ((millis() - startTime) / (1000*secDiv) == t) {// record
            //Test.record(mem_location, t, interrupt);
            interrupt = 0;
            mem_location+=2;
            t++;
            //Serial.println("finish hour");
            //Serial.println(t);
        }

        if (state == 0) { // try to request
            
            Radio.sendMessage(shortSendDuration, outMessage);
            inMessage[0] = 0; // clear msg
            Radio.receiveMessage(waitTime[w%5], inMessage, BS, userID);
            Serial.println("hi");
            Serial.println(inMessage[0]);
            Serial.println(inMessage[1]);
            Serial.println(inMessage[2]);
            Serial.println(inMessage[3]);
            
            if (inMessage[0] == bsRespond && inMessage[3] == userID && inMessage[2] == outMessage[3]) {    // receive a bs respond
                Radio.switchChannel(inMessage[1]);
                state = 1; // start sending in assigned channel
                sendFinishFlag = false;
                ref_time = millis();  // start reference time
                Serial.println("got response");
                digitalWrite(LED1, HIGH); // blue lighton your request is processed
            }
            else if (inMessage[0] == bsRequest && inMessage[3] == userID){          // receive a bs request
                outMessage[0] = cpeRespond;
                outMessage[1] = inMessage[1];
                outMessage[2] = userID;
                outMessage[3] = inMessage[2];
                Radio.sendMessage(longSendDuration, outMessage);
                Radio.switchChannel(inMessage[1]);
                state = 2;
                e = 0;
                Serial.println("new mission");
                digitalWrite(LED2, HIGH); // yellow lighton indicate other requested me
            }
            w++;
        }
        else if (state == 1) { // send stuff at assgiend channel
            //Serial.println("looping send");
            inMessage[0] = 0; // clear msg
            Radio.receiveMessage(longReceiveMaxDuration, inMessage, LBU, 0); // any LBU no need ID
            if (inMessage[0] == lbuInterrupt) {
                Serial.println("interrupted");
                interrupt++;
                state == 0;       // resume previous request
                Radio.switchChannel(0);
                outMessage[0] = cpeRequest;
                outMessage[1] = 0;
                outMessage[2] = userID;
                outMessage[3] = requestTarget%numClient; // translate to client ID
                digitalWrite(LED1, LOW); // LED1 off indicate state 0
            }
            
            if ((millis() - ref_time) <= commTime * secDiv * 1000) { // compare them in milli-second
                outMessage[0] = cpeSend;
                outMessage[1] = 0;
                outMessage[2] = userID;
                outMessage[3] = requestTarget%numClient;
                Radio.sendMessage(shortSendDuration, outMessage);
                Serial.println("sending");
            }
            else if ((millis() - ref_time) > commTime * secDiv * 1000){
                outMessage[0] = cpeDone;
                outMessage[1] = 0;
                outMessage[2] = userID;
                outMessage[3] = requestTarget%numClient;

                Radio.sendMessage(longSendDuration, outMessage);
                Serial.println("send done");
                
                requestTarget++; // new target
                state = 0;
                Radio.switchChannel(0);
                outMessage[0] = cpeRequest;
                outMessage[1] = 0;
                outMessage[2] = userID;

                while (requestTarget%numClient == 0 || requestTarget%numClient == userID){
                    requestTarget++;
                }
                outMessage[3] = requestTarget%numClient; // translate to client ID
                digitalWrite(LED1, LOW); // LED1 off indicate state 0
            }
        }
        else if (state == 2) { // listening at assgined channel
            Serial.println("listening");
            inMessage[0] = 0; // clear msg
            Radio.receiveMessage(longReceiveMaxDuration, inMessage, ANY, userID);
            Serial.println("hi");
            Serial.println(inMessage[0]);
            Serial.println(inMessage[1]);
            Serial.println(inMessage[2]);
            Serial.println(inMessage[3]);
            if (inMessage[0] == lbuInterrupt) {
                Serial.println("interrupted");
                interrupt++;
                state = 0;       // resume previous request
                Radio.switchChannel(0);
                outMessage[0] = cpeRequest;
                outMessage[1] = 0;
                outMessage[2] = userID;
                outMessage[3] = requestTarget%numClient; // translate to client ID
                digitalWrite(LED2, LOW); // LED1 off indicate state 0
            }
            else if (inMessage[0] == cpeSend) {
                Serial.println("receive send");
                e = 0;   // refresh timer
            }
            else if (e > timeExpired || inMessage[0] == cpeDone) {
                state = 0;       // resume previous request
                Radio.switchChannel(0);
                outMessage[0] = cpeRequest;
                outMessage[1] = 0;
                outMessage[2] = userID;
                outMessage[3] = requestTarget%numClient; // translate to client ID
                Serial.println("receive expired or receive done");
                digitalWrite(LED2, LOW); // LED1 off indicate state 0
            }
            e++;
        }
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(LED1,OUTPUT);
    pinMode(LED2,OUTPUT);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    //  initialize tx rx
    Radio.initialize_trans();
}

void loop() {

    if (operation == 0) {
        //digitalWrite(LED1, HIGH); // blue lighton indicate on operation 0
        //connectBaseStation();
        //digitalWrite(LED1, LOW); // blue lightout indicate finish connect and start process
        cpe_process();
        operation = 3;
    }
    else if (operation == 1) {
        // debug
    }
    else if (operation == 2) {
        // print
        Test.report();
        operation = 3;
    }
    else {
        ;
    }
}
