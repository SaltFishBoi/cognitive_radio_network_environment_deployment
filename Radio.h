/*
  Morse.h - Library for Radio transceiver function.
  Created by Kevin Z. Yu, March 23, 2021.
  Released into the public domain.
*/
#ifndef Radio_h
#define Radio_h

#include <ELECHOUSE_CC1101_SRC_DRV.h>

// 0 reserved
#define request 1
#define respond 2
#define done 3
#define ackowledge 4

class Radio
{
public:
    Radio(int pin);
    void initialize_trans();
    void receive(int maxDuration, byte message[]);
    void send(int duration, byte message[]);
private:
    int _pin;
    void decode(byte buffer[], byte message[]);
    void encode(byte message[], byte buffer[])
};

#endif