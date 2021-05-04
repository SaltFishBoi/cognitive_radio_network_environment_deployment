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

#define LED1 3
#define LED2 4

// channel status
#define FREE 0
#define OP 0xFF

// client status
#define RFS 0
#define WFC 1
#define DONE 0xFF

// TIME
unsigned long startTime = 0;              // starting reference time

// STATE
int state = 0;                  // 0 = IDEL
int operation = 0;              // 0 = experiment, 1 = output report

// SELECTION TABLE
int selectionTable[scheduleSize][maxChannel];

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
byte clientList[numClient] = { WFC, WFC, WFC, WFC};
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
    channelList[ch] = (client1 << 4) + client2;
    clientList[client1] = ch;
    clientList[client2] = ch;
}

// release when detect cpe user not in the channel for following reasons
// 1. license band user interrupt
// 2. session expired
// 3. session not successfully setup
void releaseUpdate(byte client1, byte client2, byte ch, byte st) {
    channelList[ch] = st;
    clientList[client1] = RFS;
    clientList[client2] = RFS;
}

void connectClients() {
    bool connectionFlag = false;
    Radio.switchChannel(1);         // ch 2 stand by channel before they start

    while (!connectionFlag) {
        // setup single connection
        Radio.receiveMessage(shortReceiveMaxDuration, inMessage, BS, 0);  // (... 0) dummy parameter
        if (inMessage[0] == cpeStart) {
            inMessage[0] = 0; // clear msg
            outMessage[0] = bsAcknowledge;
            outMessage[1] = 0;
            outMessage[2] = inMessage[2];
            outMessage[3] = inMessage[2];
            Radio.sendMessage(shortSendDuration, outMessage);
            releaseUpdate(inMessage[2], inMessage[2], 0, 0); // (... 0, 0) dummy parameters
            Serial.println("Connect a client");
        }

        // check if all connection success
        connectionFlag = true;
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
        Radio.receiveMessage(shortReceiveMaxDuration, inMessage, BS, 0);  // (... 0) dummy parameter
        if (inMessage[0] == lbuStart) {
            inMessage[0] = 0; // clear msg
            outMessage[0] = bsAcknowledge;
            outMessage[1] = 0;
            outMessage[2] = inMessage[2];
            outMessage[3] = inMessage[2];
            Radio.sendMessage(shortSendDuration, outMessage);
            licensedList[inMessage[2]] = 1;
        }

        // check if all connection success
        synFlag = true;
        for (byte i = 0; i < numLicensed; i++) {
            if (licensedList[i] == 0) {
                synFlag = false;
                break;
            }
        }
    }
    Serial.println("Syn with all licensed users");
    delay(3000); // count down 3 seconds
    outMessage[0] = bsStart;
    outMessage[1] = 0;
    outMessage[2] = 0;
    outMessage[3] = 0;
    Radio.sendMessage(shortSendDuration, outMessage);   // broad cast to start all time
    Radio.switchChannel(0);
}


void senseSpectrum() {
    byte clients = 0;
    byte client1 = 0;
    byte client2 = 0;
    for (byte ch = 1; ch < maxChannel; ch++) {
        Radio.switchChannel(ch);
        inMessage[0] = 0;
        Radio.receiveMessage(shortReceiveMaxDuration, inMessage, ANY, 0);
        if (inMessage[0] = FREE) { // channel FREE
            clients = getChannelOccupation(ch);
            releaseUpdate((clients >> 4), (clients & 0x0F), ch, FREE);
        }
        //else if (inMessage[0] = OP) { // channel Occupied by Priority user
        //    clients = getChannelOccupation(ch);
        //    releaseUpdate((clients >> 4), (clients & 0x0F), ch, OP);
        //}
    }
    Radio.switchChannel(0);
    Serial.println("Sense all channels");
}

void initialize_table() {
    for (byte t = 0; t < scheduleSize; t++) {
        for (byte ch = 0; ch < maxChannel; ch++) {
            if (ch == 0) {
                selectionTable[t][ch] = -1; // purposely make it the lowest
            }
            else {
                selectionTable[t][ch] = 0;
            }
        }
    }
    //selectionTable[0][0] = -1;
    //selectionTable[1][0] = -1;
    //selectionTable[2][0] = -1;
    //selectionTable[3][0] = -1;
    //selectionTable[4][0] = -1;
    //selectionTable[5][0] = -1;
    //selectionTable[6][0] = -1;
    //selectionTable[7][0] = -1;
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
byte selectChannel(byte option, unsigned long ref) {
    byte ch = 1;
    int t;
    
    if (option == 0) { // pick start from ch 1 to max channel
        while (ch <= maxChannel) {
            if (getChannelOccupation(ch) == 0) {
                break;
            }
            ch++;
        }
    }
    else if (option == 1) { // randomly pick range from ch 1 to max channel
        while (ch <= maxChannel) {
            ch = random(1, maxChannel);
            if (getChannelOccupation(ch) == 0) {
                break;
            }
        }
    }
    else if (option == 2) { // pick base on weight
        t = (((millis() - ref) / (secDiv*1000))+1)%scheduleSize;          // convert milli-second to time slot
        int sortList[maxChannel-1] = {selectionTable[t][1],
                                    selectionTable[t][2],
                                    selectionTable[t][3],
                                    selectionTable[t][4],
                                    selectionTable[t][5],
                                    selectionTable[t][6],
                                    selectionTable[t][7],
                                    selectionTable[t][8],
                                    selectionTable[t][9],
                                    selectionTable[t][10],
                                    selectionTable[t][11] };

        qsort(sortList, maxChannel, sizeof(sortList[0]), sort_desc);
        for (byte i = 0; i < maxChannel-1; i++) {
            if (getChannelOccupation(i+1) == 0) {
                break;
            }
        }
    }
    else {
        // more advance algorithm
    }

    return ch;
}

// 5/1/21 *update* remove done flag in base station
// let it run forever
void bs_process() {
    //bool doneFlag = false;
    byte r;
    byte ch;
    Radio.switchChannel(0);
    startTime = millis();

    while (true) {
        // every communication done (maybe this is not needed) we can do while true here
        //doneFlag = true;
        //for (byte i = 1; i < numClient; i++) {
        //    if (clientList[i] != DONE) {
        //        doneFlag = false;
        //        break;
        //    }
        //}

        // sense the spectrum every loop
        //senseSpectrum();

        if (state == 0) { // try to receive request
            inMessage[0] = 0; // clear msg
            Radio.receiveMessage(shortReceiveMaxDuration, inMessage, ANY, 0);
            ch = selectChannel(0, startTime);             // choose a channel for them
            if (inMessage[0] == cpeRequest) {
                //receive request, forward request
                outMessage[0] = bsRequest;
                outMessage[1] = ch;           
                outMessage[2] = inMessage[2];
                outMessage[3] = inMessage[3];
                Radio.sendMessage(shortSendDuration, outMessage);
                Serial.println("send Request");
                state = 1;
            }
        }
        else if (state == 1) { // try to receive response
            inMessage[0] = 0; // clear msg
            Radio.receiveMessage(longReceiveMaxDuration, inMessage, BS, outMessage[3]);
            if (inMessage[0] == cpeRespond) {
                //receive response, forward response back
                outMessage[0] = bsRespond;
                outMessage[1] = inMessage[1]; // assign channel
                outMessage[2] = inMessage[2];
                outMessage[3] = inMessage[3];
                Radio.sendMessage(longSendDuration, outMessage);
                occupationUpdate(inMessage[2], inMessage[3], inMessage[1]);
                Serial.println("send Respond");
                digitalWrite(LED1, HIGH); // blue lighton indicate send a respond (connection)
            }
            state = 0;
        }
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

void loop() {
    if (operation == 0) {
        //digitalWrite(LED1, HIGH); // blue lighton indicate on operation 0
        initialize_table();
        //connectClients();
        //synLicensedUsers();
        //digitalWrite(LED1, LOW); // blue lightout indicate finish all connection and yell out start
        //bs_process();
        operation = 2;
    }
    else {
        ;
    }
}
