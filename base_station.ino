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
#include <Radio.h>

//STATE
int state = 0;                  // 0 = IDEL

//FOR TRANSMIT
//byte transmitt_byte[11] = { 72,101,108,108,111,32,87,111,114,108,100 };
//char* transmitt_char = "Hello World";
int tx_time = 3;                // tx time in milli seconds
byte outMessage[2] = {0};           // message to be transmit 

//FOR RECEIVE
int rx_time = 5;                // rx time in milli seconds
int ch = 0;                     // channel number declaration and initialization
byte inMessage[3] = { 0 };           // receive message

void bs_process() {
    while (true) {
        if (state == 0) {
            Radio.receiveMessage(50, inMessage);
            if (inMessage[0] == 0) {
                //no message received
            }
            else {
                //
            }
        }
        else if (state == 1) {

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

    delayStart = millis();
    delayRunning = true;
}

void loop() {

}



