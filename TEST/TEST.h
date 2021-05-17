/*
  TEST.h - Test stimulus for this project.
  Created by Kevin Z. Yu, March 23, 2021.
  Released into the public domain.
*/

//Last edit 5/17/2021
#ifndef TEST_h
#define TEST_h

#include <Arduino.h>
#include <EEPROM.h>

#define actionSize 5
#define scheduleSize 24
#define eepromSize 1024

// change here for the test
#define numTrial 0		// set to be  the first setup of stimulus
#define secDiv 5		// 5 sec => "1 day"
//#define commTime 1	    	// (1)*secDev = 5 sec
#define commTimeInc 0
#define endInDay 7		// run for "7 days", max is "64 days", with limitation on the selection table "64/2/commTime"
#define alg 0			// 0 = inorder, 1 random, 2 selection

// base station capacity
#define maxChannel 12		// channel 1-11 for assignment, channel 0 for setup communication
#define numClient 6		// number of clients are actaully 5 in this case, maxClient is actually 0x0F
				// changing this number require a change in array initialization at bs
#define numLicensed 3

// current trialNum
#define trialNum 0
// mannuel actions for cpe
//const byte actionsTrial[1][5][5] = {{ {0x32, 0x32, 0x32, 0x32, 0x32},
//                      	      {0x32, 0x32, 0x32, 0x32, 0x32},
//                    		      {0x32, 0x32, 0x32, 0x32, 0x32},
//                    		      {0x32, 0x32, 0x32, 0x32, 0x32},
//                    		      {0x32, 0x32, 0x32, 0x32, 0x32} }};

const byte schedulesTrial[1][3][24] = {{ {0x00, 0x01, 0x02, 0x12, 0x12, 0x12, 0x9a, 0x9a, 0x9a, 0x00, 0x14, 0x23, 0xa3, 0x94, 0x28, 0x24, 0x5b, 0x39, 0x71, 0x16, 0x6a, 0x95, 0xa2, 0x93},
                       			 {0x00, 0x00, 0x00, 0x34, 0x45, 0x56, 0xb1, 0xb1, 0xb1, 0x00, 0x69, 0x59, 0xa6, 0xa4, 0x7b, 0x45, 0x16, 0xab, 0x89, 0x52, 0x12, 0x18, 0x21, 0x41},
                   	  	         {0x00, 0x00, 0x00, 0x56, 0x67, 0x78, 0x23, 0x23, 0x23, 0x00, 0x53, 0x12, 0x98, 0x23, 0x64, 0x39, 0x24, 0x71, 0x34, 0x43, 0xa4, 0xb6, 0x59, 0x38}}};

//					  none, 1ch , xch ,1/2ch,3free,34fre, long lasting occ, rest, all random ...
class TEST
{
public:
    //RADIO(int pin);
    //void initialize_actionList(byte id, byte *action);
    void initialize_scheduleList(byte id, byte *schedule);
    void record(int location, byte time, byte data);
    void report(void);

};

extern TEST Test;

#endif