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

//FOR TRANSMIT
//byte transmitt_byte[11] = {72,101,108,108,111,32,87,111,114,108,100};
//char* transmitt_char = "Hello World";
int tx_time = 3;                // tx time in milli seconds
byte buffer[2] = {0};           // buffer to be transmit

void lbu_process() {
    while (true) {
        if (state == 0) {
            //message = Radio.receiveMessage(50);
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

void loop(){

}



