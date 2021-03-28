/*
  TEST.h - Test stimulus for this project.
  Created by Kevin Z. Yu, March 23, 2021.
  Released into the public domain.
*/
#ifndef TEST_h
#define TEST_h

#include <Arduino.h>
#include <EEPROM.h>

#define actionSize 5
#define scheduleSize 24
#define eepromSize 512

// current trialNum
#define trialNum 0
const actions1 = { {0x32, 0x32, 0x32, 0x32, 0x32},
                 {0x32, 0x32, 0x32, 0x32, 0x32},
                 {0x32, 0x32, 0x32, 0x32, 0x32},
                 {0x32, 0x32, 0x32, 0x32, 0x32},
                 {0x32, 0x32, 0x32, 0x32, 0x32} };

const schedules1 = { {0, 0x01, 0x01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0x01, 0x01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0x01, 0x01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0x01, 0x01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0x01, 0x01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0x01, 0x01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} };

const actionsTrial = {actions1};
const schedulesTrial = { schedules1 };

class TEST
{
public:
    //RADIO(int pin);
    void initialize_actionList(byte id, byte *action);
    void initialize_scheduleList(byte id, byte *schedule);
    //void record();
    void report(void);

};

extern TEST Test;

#endif