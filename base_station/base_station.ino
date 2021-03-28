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
#include <TEST.h>

// base station capacity
#define maxChannel 12       // channel 1-11 for assignment, channel 0 for setup communication
#define numClient 6         // number of clients are actaully 5 in this case, maxClient is actually 0x0F
#define numLicense 3

// channel status
#define FREE 0
#define OP 0xFF

// client status
#define RFS 0
#define WFC 1
#define DONE 0xFF

// TIME
int startTime = 0;              // starting reference time

// STATE
int state = 0;                  // 0 = IDEL

// SELECTION TABLE
int selectionTable[24][maxChannel];

// CHANNEL LIST
// channel zero reserved
// status 0 = FREE, status 0xFF Occupied by Priority user
// record channel occupation
// example: {  0  , 0x12,  0  , 0x34, ...}
//          | CH0 | CH1 | CH2 | CH3 ...
byte channelList[maxChannel] = { FREE };
// CLIENT LIST
// client zero reserved
// status 0 = Requesting For Session, status 1 = Waiting For Connection, status 0xFF = DONE
// record client channel status
// example: {    0    ,    1    ,    1    ,    3     ,...}
//          | client0 | client1 | client2 | client3   ...
byte clientList[numClient] = { WFC };
byte licensedList[numLicensed] = { 0 };

// -----------------------------------------------------
// |   opCode   |  payload   |     src    |     des    |
// -----------------------------------------------------
// | message[0] | message[1] | message[2] | message[3] |
// -----------------------------------------------------
byte outMessage[4] = {0};           // message to be transmit 
byte inMessage[4] = {0};           // receive message

byte getClientStatus(byte clientID) {
    return clientList[clientID];
}

byte getChannelOccupation(byte ch) {
    return channelList[ch];
}

void occupationUpdate(byte client1, byte client2, byte ch) {
    channelList[ch] == (client1 << 4) + client2;
    clientList[client1] == ch;
    clientList[client2] == ch;
}

// release when detect cpe user not in the channel for following reasons
// 1. license band user interrupt
// 2. session expired
// 3. session not successfully setup
void releaseUpdate(byte client1, byte client2, byte ch, byte status) {
    channelList[ch] == status;
    clientList[client1] == RFS;
    clientList[client2] == RFS;
}

void connectClients() {
    bool connectionFlag = false;
    Radio.switchChannel(1);         // ch 1 stand by channel before they start

    while (!connectionFlag) {
        // setup single connection
        Radio.receiveMessage(bsReceiveMaxDuration, inMessage, BS, 0);  // (... 0) dummy parameter
        if (inMessage[0] == cpeStart) {
            outMessage[0] = bsAckowledge;
            outMessage[1] = 0;
            outMessage[2] = inMessage[2];
            outMessage[3] = inMessage[2];
            Radio.sendMessage(bsSendDuration, outMessage);
            releaseUpdate(inMessage[2], inMessage[2], 0, 0) // (... 0, 0) dummy parameters
        }

        // check if all connection success
        conenctionFlag = true;
        for (byte i = 1; i < numClient; i++) {
            if (clientList[i] == WFC) {
                connectionFlag = false;
                break;
            }
        }
    }
    Serial.println("Connect all clients");
}

void synLicensedUsers() {
    bool synFlag = false;
    Radio.switchChannel(2);         // ch 2 stand by channel before they start

    while (!synFlag) {
        // setup single connection
        Radio.receiveMessage(bsReceiveMaxDuration, inMessage, BS, 0);  // (... 0) dummy parameter
        if (inMessage[0] == lbuStart) {
            outMessage[0] = bsAckowledge;
            outMessage[1] = 0;
            outMessage[2] = inMessage[2];
            outMessage[3] = inMessage[2];
            Radio.sendMessage(bsSendDuration, outMessage);
            licenseList[inMessage[2]] = 1;
        }

        // check if all connection success
        synFlag = true;
        for (byte i = 1; i < numLicense; i++) {
            if (licenseList[i] == 0) {
                synFlag = false;
                break;
            }
        }
    }
    Serial.println("Syn with all licensed users");
    outMessage[0] = bsStart;
    outMessage[1] = 0;
    outMessage[2] = 0;
    outMessage[3] = 0;
    Radio.sendMessage(bsSendDuration, outMessage);   // broad cast to start all time
    Radio.switchChannel(0);
}


void senseSpectrum() {
    byte clients = 0;
    byte client1 = 0;
    byte client2 = 0;
    for (byte ch = 1; ch < maxChannel; ch++) {
        Radio.switchChannel(ch);
        Radio.receiveMessage(bsReceiveMaxDuration, inMessage, BS, 0);
        if (inMessage[0] = FREE) { // channel FREE
            clients = getChannelOccupation(ch);
            releaseUpdate((clients >> 4), (clients & 0x0F), ch, FREE);
        }
        else if (inMessage[0] = OP) { // channel Occupied by Priority user
            clients = getChannelOccupation(ch);
            releaseUpdate((clients >> 4), (clients & 0x0F), ch, OP);
        }
    }
    Radio.switchChannel(0);
    Serial.println("Sense all channels");
}

void initialize_table() {
    for (byte 1 = 0; t < 24; t++) {
        for (byte ch = 0; ch < maxChannel; ch++) {
            if (ch == 0) {
                selectionTable[t][ch] = -1;
            }
            else {
                selectionTable[t][ch] = 0;
            }
        }
    }
    selectionTable[0][0] = -1;
    selectionTable[1][0] = -1;
    selectionTable[2][0] = -1;
    selectionTable[3][0] = -1;
    selectionTable[4][0] = -1;
    selectionTable[5][0] = -1;
    selectionTable[6][0] = -1;
    selectionTable[7][0] = -1;
}

//https://arduino.stackexchange.com/questions/38177/how-to-sort-elements-of-array-in-arduino-code
int sort_desc(const void* cmp1, const void* cmp2)
{
    // Need to cast the void * to int *
    int a = *((int*)cmp1);
    int b = *((int*)cmp2);
    // The comparison
    return a > b ? -1 : (a < b ? 1 : 0);
    // A simpler, probably faster way:
    //return b - a;
}

// channel selection algorithm 
byte selectChannel(byte option, int ref) {
    byte ch = 1;
    int time;
    if (option == 0) { // pick start from ch 1 to max channel
        while (true) {
            if (getChannelOccupation(ch) == 0) {
                break
            }
            ch++;
        }
    }
    else if (option == 1) { // randomly pick range from ch 1 to max channel
        while (true) {
            ch = random(1, maxChannel);
            if (getChannelOccupation(ch) == 0) {
                break
            }
        }
    }
    else if (option == 2) { // pick base on weight
        time = (millis() - ref) / 60000;          // convert milli-second to min
        int sortList[maxChannel] = { selectionTable[time][0],
                                    selectionTable[time][1],
                                    selectionTable[time][2],
                                    selectionTable[time][3],
                                    selectionTable[time][4],
                                    selectionTable[time][5],
                                    selectionTable[time][6],
                                    selectionTable[time][7],
                                    selectionTable[time][8],
                                    selectionTable[time][9],
                                    selectionTable[time][10],
                                    selectionTable[time][11] };

        qsort(sortList, maxChannel, sizeof(sortList[0]), sort_desc);
        for (byte i = 1; i < maxChannel; i++) {
            ch = sortList[i];
            if (getChannelOccupation(ch) == 0) {
                break;
            }
        }
    }
    else {
        // more advance algorithm
    }

    return ch;
}

void bs_process() {
    bool doneFlag = false;
    Radio.switchChannel(0);
    startTime = millis();


    while (!doneFlag) {
        // every communication done
        doneFlag = true;
        for (byte i = 1; i < numClient; i++) {
            if (clientList[i] != DONE) {
                doneFlag = false;
                break;
            }
        }

        if (state == 0) { // try to receive request
            Radio.receiveMessage(bsReceiveMaxDuration, inMessage, BS, 0);
            if (inMessage[0] == 0) {
                //no message received
            }
            else if (inMessage[0] == cpeRequest) {
                //receive request, forward request
                outMessage[0] = bsRespond;
                outMessage[1] = selectChannel(0, startTime);                  // choose a channel for them
                outMessage[2] = inMessage[2];
                outMessage[3] = inMessage[3];
                Radio.sendMessage(bsSendDuration, outMessage);
                state = 1;
            }
            else {
                //
            }
        }
        else if (state == 1) { // try to receive response
            Radio.receiveMessage(bsReceiveMaxDuration, inMessage, BS, 0);
            if (inMessage[0] == 0) {
                //no message received
            }
            else if (inMessage[0] == cpeRespond) {
                //receive response, forward response back
                outMessage[0] = bsRespond;
                outMessage[1] = inMessage[1]; // assign channel
                outMessage[2] = inMessage[2];
                outMessage[3] = inMessage[3];
                Radio.sendMessage(bsSendDuration, outMessage);
                occupationUpdate(inMessage[2], inMessage[3], inMessage[1]);
            }
            else {
                //
            }
            state = 0;
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
    Radio.receiveMessage(bsReceiveMaxDuration, inMessage, 0, 0);
    Serial.println(inMessage[0]);
    Serial.println(inMessage[1]);
    Serial.println(inMessage[2]);
    Serial.println(inMessage[3]);

    Serial.println();
}
